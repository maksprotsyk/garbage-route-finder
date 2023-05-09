//
// Created by Максим Процик on 29.04.2023.
//

#include "Problem.h"
#include "Arguments.h"

#include <nlohmann/json.hpp>

#include "osrm/match_parameters.hpp"
#include "osrm/nearest_parameters.hpp"
#include "osrm/route_parameters.hpp"
#include "osrm/table_parameters.hpp"
#include "osrm/trip_parameters.hpp"

#include "osrm/coordinate.hpp"
#include "osrm/engine_config.hpp"
#include "osrm/json_container.hpp"

#include "osrm/osrm.hpp"
#include "osrm/status.hpp"

Problem* Problem::m_problem = nullptr;

Problem &Problem::GetProblem()
{
    if (!m_problem)
    {
        m_problem = new Problem;
    }
    return *m_problem;
}

void Problem::ReadProblem(const std::string &path)
{
    m_garbageTrucks.clear();
    m_containers.clear();
    m_landfills.clear();

    std::ifstream file(path);

    nlohmann::json json;
    file >> json;

    auto json_description = json["description"];

    if (!json_description.is_null())
    {
        m_description = json_description;
    }
    else
    {
        m_description = "";
    }

    auto containers_json = json["containers"];
    for (auto& container_info: containers_json)
    {
        m_containers.emplace_back(
                        Location(container_info["latitude"].get<double>(), container_info["longitude"].get<double>()),
                        TimeWindow(container_info["start"].get<double>(), container_info["finish"].get<double>())
        );
        m_containers.back().SetTypeData(m_containerTypes[container_info["type"].get<std::string>()]);
    }

    auto trucks_json = json["trucks"];
    for (auto& truck_info: trucks_json) {
        m_garbageTrucks.emplace_back(
                Location(truck_info["latitude"].get<double>(), truck_info["longitude"].get<double>()),
                TimeWindow(truck_info["start"].get<double>(), truck_info["finish"].get<double>())
        );
        m_garbageTrucks.back().SetTypeData(m_truckTypes[truck_info["type"].get<std::string>()]);
    }

    auto landfills_json = json["landfills"];
    for (auto& landfill_info: landfills_json) {
        m_landfills.emplace_back(
                Location(landfill_info["latitude"].get<double>(), landfill_info["longitude"].get<double>()),
                TimeWindow(landfill_info["start"].get<double>(), landfill_info["finish"].get<double>())
        );
    }

    Location::ClearCache();

    bool realDistances = Arguments::GetArguments().GetValue<bool>(Arguments::RealDistances);
    Location::REAL_DISTANCES = realDistances;

    if (realDistances)
    {
        CalculateRealDistances();
    }
}

void Problem::CalculateRealDistances()
{
    using namespace osrm;
    EngineConfig config;

    std::string mapFile = Arguments::GetArguments().GetValue<std::string>(Arguments::MapFile);
    config.storage_config = {mapFile};
    config.use_shared_memory = false;
    config.algorithm = EngineConfig::Algorithm::MLD;

    const OSRM osrm{config};

    TableParameters params;

    std::vector<Location> locations;

    for (const auto& cont: m_containers) {
        locations.push_back(cont.GetLocation());
    }

    for (const auto& truck: m_garbageTrucks) {
        locations.push_back(truck.GetLocation());
    }

    for (const auto& landfill: m_landfills) {
        locations.push_back(landfill.GetLocation());
    }

    for (const auto& loc: locations)
    {
        params.coordinates.emplace_back(
                util::FloatLongitude{loc.GetLongitude()},
                util::FloatLatitude{loc.GetLatitude()}

        );
    }

    params.annotations = engine::api::TableParameters::AnnotationsType::Distance;

    engine::api::ResultT result = json::Object();


    const auto status = osrm.Table(params, result);

    auto &json_result = result.get<json::Object>();
    if (status == Status::Ok)
    {
        auto &durations = json_result.values["distances"].get<json::Array>();
        int size = durations.values.size();

        for (int i = 0; i < size; i++) {
            auto &row = durations.values.at(i).get<json::Array>();
            Location loc1 = locations[i];
            for (int j = 0; j < size; j++) {
                Location loc2 = locations[j];
                double distance = row.values.at(j).get<json::Number>().value / 1000.0;

                Location::AddCachedDistance(loc1, loc2, distance);
            }
        }
    }



}

void Problem::ReadConfig(const std::string &path)
{
    m_containerTypes.clear();
    m_truckTypes.clear();

    std::ifstream file(path);

    nlohmann::json json;
    file >> json;

    auto container_types_json = json["container_types"];
    for (const auto& container_type: container_types_json) {
        Container::TypeData containerData;
        containerData.capacity = container_type["capacity"];
        containerData.processingTime = container_type["processing_time"];
        containerData.garbageType = container_type["garbage_type"];
        containerData.typeName = container_type["type"];
        m_containerTypes[containerData.typeName] = containerData;

    }

    auto truck_types_json = json["truck_types"];
    for (const auto& truck_type: truck_types_json) {
        GarbageTruck::TypeData truckData;
        truckData.typeName = truck_type["type"];
        truckData.speed = truck_type["speed"];
        truckData.fuelConsumption = truck_type["fuel_consumption"];
        truckData.fuelCapacity = truck_type["fuel_capacity"];
        truckData.unloadingTime = truck_type["unloading_time"];
        truckData.capacities = truck_type["capacities"].get<std::unordered_map<std::string, double>>();
        m_truckTypes[truckData.typeName] = truckData;

    }

}

size_t Problem::GetContainersNum() const
{
    return m_containers.size();
}

size_t Problem::GetLandfillsNum() const
{
    return m_landfills.size();
}

size_t Problem::GetTrucksNum() const
{
    return m_garbageTrucks.size();
}

const Container &Problem::GetContainer(ObjectId i) const
{
    return m_containers[i];
}

const Landfill &Problem::GetLandfill(ObjectId i) const
{
    return m_landfills[i];
}

const GarbageTruck &Problem::GetTruck(ObjectId i) const
{
    return m_garbageTrucks[i];
}
