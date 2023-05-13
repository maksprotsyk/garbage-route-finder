//
// Created by Максим Процик on 30.04.2023.
//

#include "Solution.h"

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

Solution::Solution()
{
    Problem& problem = Problem::GetProblem();
    for (ObjectId id = 0; id < problem.GetContainersNum(); id++)
    {
        Orphans.insert(id);
    }
    for (ObjectId id = 0; id < problem.GetTrucksNum(); id++)
    {
        UnusedTrucks.insert(id);
    }
}

bool Solution::CheckSolution()
{
    std::set<ObjectId> allContainerIds;
    for (ObjectId id: Orphans)
    {
        allContainerIds.insert(id);
    }
    std::set<ObjectId> allTrucks;
    for (ObjectId id: UnusedTrucks)
    {
        allTrucks.insert(id);
    }
    Problem& problem = Problem::GetProblem();

    for (int j = 0; j < Routes.size(); j++)
    {
        Route& route = Routes[j];

        if (route.Exportations.empty())
        {
            return false;
        }
        if (route.Truck < 0)
        {
            return false;
        }

        if (!allTrucks.insert(route.Truck).second)
        {
            return false;
        }


        if (route.Truck != route.Exportations[0].Truck)
        {
            return false;
        }

        for (ObjectId container: route.Exportations[0].Containers)
        {
            if (container < 0) {
                return false;

            }

            if (!allContainerIds.insert(container).second) {
                return false;
            }

        }

        if (route.Exportations[0].CurrentLandfill < 0)
        {
            return false;
        }

        const GarbageTruck& truck = problem.GetTruck(route.Truck);
        if (
                !(route.Exportations[0].StartLocation == truck.GetLocation()) ||
                route.Exportations[0].StartDistance != 0 ||
                route.Exportations[0].StartTime != truck.GetTimeWindow().GetStart()
                )
        {
            return false;
        }
        bool res = route.Exportations[0].RecalculateInnerValues(0);
        if (!res)
        {
            return false;
        }
        res = route.Exportations[0].RecalculateFinishValues();
        if (!res)
        {
            return false;
        }

        for (int i = 1; i < route.Exportations.size(); i++)
        {

            if (route.Truck != route.Exportations[i].Truck)
            {
                return false;
            }

            for (auto container: route.Exportations[i].Containers)
            {
                if (container < 0) {
                    return false;

                }

                if (!allContainerIds.insert(container).second)
                {
                     return false;
                }

            }

            if (route.Exportations[i].CurrentLandfill < 0)
            {
                return false;
            }
            const Landfill& landfill = problem.GetLandfill(route.Exportations[i].CurrentLandfill);

            if (
                    route.Exportations[i].StartTime != route.Exportations[i-1].GetFinishTime() ||
                    !(route.Exportations[i].StartLocation == landfill.GetLocation()) ||
                    route.Exportations[i].StartDistance != route.Exportations[i-1].GetFinishDistance()
                    )
            {
                return false;
            }


            res = route.Exportations[i].RecalculateInnerValues(0);
            if (!res)
            {
                return false;
            }

            if (!route.Exportations[i].RecalculateFinishValues())
            {
                return false;
            }

        }

        res = route.CalculateFinishValues();
        if (!res)
        {
            return false;
        }

    }


    if (allContainerIds.size() != problem.GetContainersNum())
    {
        return false;
    }

    if (allTrucks.size() != problem.GetTrucksNum())
    {
        return false;
    }

    return true;
}

double Solution::CalculateCost() const
{
    double fuelCoef = 1;
    double orphanCoef = 100;
    double cost = 0;

    cost += orphanCoef * Orphans.size();
    cost += fuelCoef * CalculateTotalFuel();

    return cost;
}

double Solution::CalculateTotalDistance() const
{
    double total = 0;
    for (const auto& route: Routes)
    {
        double distance = route.FinishDistance();
        total += distance;
    }
    return total;
}

double Solution::CalculateTotalTime() const
{
    Problem& problem = Problem::GetProblem();
    double endTime = 0;
    double startTime = 100000000;
    for (const auto& route: Routes)
    {
        double finishTime = route.FinishTime();
        double truckStartTime = problem.GetTruck(route.Truck).GetTimeWindow().GetStart();
        startTime = std::min(startTime, truckStartTime);
        endTime = std::max(endTime, finishTime + truckStartTime);
    }
    return endTime - startTime;
}

double Solution::CalculateTotalFuel() const
{
    double total = 0;
    for (const auto& route: Routes)
    {
        double fuel = route.FinishFuel();
        total += fuel;
    }
    return total;
}

std::vector<ObjectId> Solution::PerformAction(const DeleteAction &action)
{
    if (Routes.size() <= action.route_index)
    {
        return {};
    }
    Route& route = Routes[action.route_index];
    if (route.Exportations.size() <= action.exportation_index)
    {
        return {};
    }
    Exportation& exportation = route.Exportations[action.exportation_index];
    if (action.end < action.start)
    {
        return {};
    }
    if (action.end >= exportation.Containers.size())
    {
        return {};
    }

    std::vector<ObjectId> deleted;
    for (int i = action.start; i <= action.end; i++)
    {
        ObjectId id = exportation.Containers[action.start];
        exportation.Containers.erase(exportation.Containers.begin() + action.start);
        Orphans.insert(id);
        deleted.push_back(id);
    }
    exportation.RecalculateInnerValues(action.start);
    exportation.RecalculateFinishValues();
    for (int i = action.exportation_index + 1; i < route.Exportations.size(); i++)
    {
        route.Exportations[i].StartTime = route.Exportations[i-1].GetFinishTime();
        route.Exportations[i].StartDistance = route.Exportations[i-1].GetFinishDistance();
        route.Exportations[i].RecalculateInnerValues(0);
        route.Exportations[i].RecalculateFinishValues();
    }
    route.CalculateFinishValues();
    InvalidateHash();
    return deleted;

}

bool Solution::PerformAction(const InsertAction &action)
{
    if (Routes.size() <= action.route_index)
    {
        return false;
    }
    Route& route = Routes[action.route_index];
    if (route.Exportations.size() <= action.exportation_index)
    {
        return false;
    }
    Exportation& exportation = route.Exportations[action.exportation_index];
    if (action.start > exportation.Containers.size())
    {
        return false;
    }

    exportation.Containers.insert(exportation.Containers.begin() + action.start, action.ids.begin(), action.ids.end());

    bool res = true;
    if (exportation.RecalculateInnerValues(action.start) && exportation.RecalculateFinishValues())
    {
        for (size_t i = action.exportation_index + 1; i < route.Exportations.size(); i++)
        {
            if (!route.Exportations[i].RecalculateInnerValues(0) || !route.Exportations[i].RecalculateFinishValues())
            {
                res = false;
                break;
            }
        }
        if (res)
        {
            res = route.CalculateFinishValues();
        }
    }
    else
    {
        res = false;
    }

    if (!res)
    {
        exportation.Containers.erase(
                exportation.Containers.begin() + action.start,
                exportation.Containers.begin() + action.start + action.ids.size()
                );
        return false;
    }

    for (ObjectId id: action.ids)
    {
        Orphans.erase(id);
    }
    InvalidateHash();
    return true;
}


size_t Solution::CalculateHash() const
{
    size_t hash = 0;
    for (const auto& route: Routes)
    {
        boost::hash_combine(hash, route.Truck);
        for (const auto& exportation: route.Exportations)
        {
            boost::hash_combine(hash, exportation.CurrentLandfill);
            for (ObjectId cont: exportation.Containers)
            {
                boost::hash_combine(hash, cont);
            }
        }
    }
    return hash;
}



std::vector<Location> Solution::FindRealPath(const Location &start, const Location &end, bool realDistances)
{
    if (!realDistances)
    {
        return std::vector<Location>{start, end};
    }

    using namespace osrm;
    EngineConfig config;
    config.storage_config = {Arguments::GetArguments().GetValue<std::string>(Arguments::MapFile)};
    config.use_shared_memory = false;
    config.algorithm = EngineConfig::Algorithm::MLD;

    const OSRM osrm{config};

    RouteParameters params;
    params.steps = true;

    params.coordinates.emplace_back(util::FloatLongitude{start.GetLongitude()}, util::FloatLatitude{start.GetLatitude()});
    params.coordinates.emplace_back(util::FloatLongitude{end.GetLongitude()}, util::FloatLatitude{end.GetLatitude()});

    engine::api::ResultT result = json::Object();

    const auto status = osrm.Route(params, result);

    auto &json_result = result.get<json::Object>();

    if (status != Status::Ok) {
        const auto code = json_result.values["code"].get<json::String>().value;
        const auto message = json_result.values["message"].get<json::String>().value;

        std::cout << "Code: " << code << "\n";
        std::cout << "Message: " << code << "\n";
        return std::vector<Location>();
    }

    std::vector<Location> path;
    auto &route = json_result.values["routes"].get<json::Array>().values.at(0).get<json::Object>();
    auto &leg = route.values["legs"].get<json::Array>().values.at(0).get<json::Object>();
    auto &steps = leg.values["steps"].get<json::Array>();
    for (auto &value : steps.values) {
        auto &step = value.get<json::Object>();
        auto intersections = step.values["intersections"].get<json::Array>();
        for (auto &intersection_obj : intersections.values) {
            auto &intersection = intersection_obj.get<json::Object>();
            auto location = intersection.values["location"].get<json::Array>();
            path.emplace_back(
                    location.values.at(1).get<json::Number>().value,
                    location.values.at(0).get<json::Number>().value);
            //std::cout << location.values.at(0).get<json::Number>().value << " ";
            //std::cout << location.values.at(1).get<json::Number>().value << std::endl;

        }
    }

    return path;
}

std::ostream &operator<<(std::ostream &stream, const Solution &solution)
{
    size_t width = stream.width();
    std::string line(width, ' ');
    stream << line << "Solution(\n" << line << "--Orhapns: {";

    for (ObjectId id: solution.Orphans) {
        stream << id << " ";
    }
    stream << "}\n";
    stream << line << "--Unused trucks: {";
    for (ObjectId id: solution.UnusedTrucks) {
        stream << id << " ";
    }
    stream << "}\n";

    stream << line << "--Routes: {\n";

    for (const Route& route: solution.Routes) {
        stream << std::setw(width + 4) << route << ",\n";
    }
    stream << line << "--}\n";
    stream << line << ")";


    return stream;

}


bool operator==(const Solution& sol1, const Solution& sol2)
{
   if (sol1.Routes.size() != sol2.Routes.size())
   {
       return false;
   }
   for (size_t i = 0; i < sol1.Routes.size(); i++)
   {
       const Route& route1 = sol1.Routes[i];
       const Route& route2 = sol2.Routes[i];

       if (route1.Truck != route2.Truck)
       {
           return false;
       }

       if (route1.Exportations.size() != route2.Exportations.size())
       {
           return false;
       }
       for (size_t j = 0; j < route1.Exportations.size(); j++)
       {
           const Exportation& exp1 = route1.Exportations[i];
           const Exportation& exp2 = route2.Exportations[i];
           if (exp1.CurrentLandfill != exp2.CurrentLandfill)
           {
               return false;
           }
           if (exp1.Containers.size() != exp2.Containers.size())
           {
               return false;
           }

           for (size_t c = 0; c < exp1.Containers.size(); c++)
           {
               if (exp1.Containers[c] != exp2.Containers[c])
               {
                   return false;
               }
           }
       }
   }

   return true;

}


nlohmann::json Solution::CreateJson() const
{
    nlohmann::json solutionJson;

    solutionJson["TotalDistance"] = CalculateTotalDistance();
    solutionJson["TotalTime"] = CalculateTotalTime();
    solutionJson["TotalFuel"] = CalculateTotalFuel();
    nlohmann::json orphansArray = nlohmann::json::array();

    Problem& problem = Problem::GetProblem();

    for (size_t id: Orphans)
    {
        nlohmann::json orphan;
        const Container& container = problem.GetContainer(id);
        orphan["Longitude"] = container.GetLocation().GetLongitude();
        orphan["Latitude"] = container.GetLocation().GetLatitude();
        orphan["Type"] = container.GetType();
        orphansArray.emplace_back(orphan);
    }

    solutionJson["Orphans"] = orphansArray;

    bool realDistances = Arguments::GetArguments().GetValue<bool>(Arguments::RealDistances);
    nlohmann::json routesJson;
    for (const Route& route: Routes)
    {
        nlohmann::json routeJson;
        nlohmann::json truckJson;
        const GarbageTruck& truck = problem.GetTruck(route.Truck);

        truckJson["Longitude"] = truck.GetLocation().GetLongitude();
        truckJson["Latitude"] = truck.GetLocation().GetLatitude();
        truckJson["Type"] = truck.GetType();
        routeJson["Truck"] = truckJson;

        nlohmann::json routePoints = nlohmann::json::array();
        nlohmann::json firstPoint = {
                {"Longitude", truck.GetLocation().GetLongitude()},
                {"Latitude", truck.GetLocation().GetLatitude()}
        };
        routePoints.emplace_back(firstPoint);
        Location prevLocation = truck.GetLocation();
        for (const Exportation& exportation: route.Exportations)
        {
            for (ObjectId id: exportation.Containers)
            {
                const Container& container = problem.GetContainer(id);
                auto path = FindRealPath(prevLocation, container.GetLocation(), realDistances);
                for (int i = 0; i+1 < path.size(); i++)
                {
                    nlohmann::json point = {
                            {"Longitude", path[i].GetLongitude()},
                            {"Latitude", path[i].GetLatitude()}
                    };
                    routePoints.emplace_back(point);
                }
                prevLocation = container.GetLocation();
            }
            const Landfill& landfill = problem.GetLandfill(exportation.CurrentLandfill);
            auto path = FindRealPath(prevLocation, landfill.GetLocation(), realDistances);
            for (int i = 0; i+1 < path.size(); i++)
            {
                nlohmann::json point = {
                        {"Longitude", path[i].GetLongitude()},
                        {"Latitude", path[i].GetLatitude()}
                };
                routePoints.emplace_back(point);
            }
            prevLocation = landfill.GetLocation();
        }
        auto path = FindRealPath(prevLocation, truck.GetLocation(), realDistances);
        for (int i = 0; i < path.size(); i++)
        {
            nlohmann::json point = {
                    {"Longitude", path[i].GetLongitude()},
                    {"Latitude", path[i].GetLatitude()}
            };
            routePoints.emplace_back(point);
        }
        routeJson["Points"] = routePoints;
        routesJson.emplace_back(routeJson);
    }

    solutionJson["Routes"] = routesJson;

    nlohmann::json containersJson = nlohmann::json::array();
    for (ObjectId id = 0; id < problem.GetContainersNum(); id++)
    {
        const Container& container = problem.GetContainer(id);
        nlohmann::json containerJson = {
                {"Longitude", container.GetLocation().GetLongitude()},
                {"Latitude", container.GetLocation().GetLatitude()},
                {"Type", container.GetType()}
        };
        containersJson.emplace_back(containerJson);
    }
    solutionJson["Containers"] = containersJson;

    nlohmann::json trucksJson = nlohmann::json::array();
    for (ObjectId id = 0; id < problem.GetTrucksNum(); id++)
    {
        const auto& truck = problem.GetTruck(id);
        nlohmann::json truckJson = {
                {"Longitude", truck.GetLocation().GetLongitude()},
                {"Latitude", truck.GetLocation().GetLatitude()},
                {"Type", truck.GetType()}
        };
        trucksJson.emplace_back(truckJson);
    }
    solutionJson["Trucks"] = trucksJson;

    nlohmann::json landfillsJson = nlohmann::json::array();
    for (ObjectId id = 0; id < problem.GetLandfillsNum(); id++)
    {
        const auto& landfill = problem.GetLandfill(id);
        nlohmann::json landfillJson = {
                {"Longitude", landfill.GetLocation().GetLongitude()},
                {"Latitude", landfill.GetLocation().GetLatitude()}
        };
        landfillsJson.emplace_back(landfillJson);
    }
    solutionJson["Landfills"] = landfillsJson;
    solutionJson["Description"] = Description;
    solutionJson["OrphansCount"] = Orphans.size();
    solutionJson["UnusedTrucksCount"] = UnusedTrucks.size();

    return solutionJson;
}
