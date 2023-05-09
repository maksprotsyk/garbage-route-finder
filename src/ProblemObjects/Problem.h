//
// Created by Максим Процик on 29.04.2023.
//

#ifndef ROUTING_PROBLEM_PROBLEM_H
#define ROUTING_PROBLEM_PROBLEM_H

#include <string>
#include <vector>
#include <fstream>

#include "GarbageTruck.h"
#include "Container.h"
#include "Landfill.h"

using ObjectId = int;

class Problem
{
public:
    static Problem& GetProblem();
    void ReadProblem(const std::string& path);
    void ReadConfig(const std::string& path);

    [[nodiscard]] size_t GetContainersNum() const;
    [[nodiscard]] size_t GetLandfillsNum() const;
    [[nodiscard]] size_t GetTrucksNum() const;

    [[nodiscard]] const Container& GetContainer(ObjectId i) const;
    [[nodiscard]] const Landfill& GetLandfill(ObjectId i) const;
    [[nodiscard]] const GarbageTruck& GetTruck(ObjectId i) const;

private:
    static Problem* m_problem;
    Problem() = default;
    std::vector<Container> m_containers;
    std::vector<GarbageTruck> m_garbageTrucks;
    std::vector<Landfill> m_landfills;
    std::string m_description = "";

    std::unordered_map<std::string, Container::TypeData> m_containerTypes;
    std::unordered_map<std::string, GarbageTruck::TypeData> m_truckTypes;

    void CalculateRealDistances();
};


#endif //ROUTING_PROBLEM_PROBLEM_H
