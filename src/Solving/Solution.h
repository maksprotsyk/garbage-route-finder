//
// Created by Максим Процик on 30.04.2023.
//

#ifndef ROUTING_PROBLEM_SOLUTION_H
#define ROUTING_PROBLEM_SOLUTION_H

#include "Problem.h"
#include "Route.h"
#include "Arguments.h"
#include "HashableObject.h"

#include <vector>
#include <set>
#include <algorithm>
#include <queue>
#include <unordered_map>
#include <set>
#include <nlohmann/json.hpp>


struct DeleteAction {
    size_t route_index;
    size_t exportation_index;
    size_t start;
    size_t end;
};


struct InsertAction {
    size_t route_index;
    size_t exportation_index;
    size_t start;
    std::vector<ObjectId> ids;
};

class Solution: public HashableObject {
public:
    std::string Description;
    std::string Name;

    std::vector<Route> Routes;
    std::set<ObjectId> Orphans;
    std::set<ObjectId> UnusedTrucks;

    Solution();

    bool CheckSolution();

    double CalculateCost() const;

    double CalculateTotalFuel() const;

    double CalculateTotalDistance() const;

    double CalculateTotalTime() const;

    std::vector<ObjectId> PerformAction(const DeleteAction& action);

    bool PerformAction(const InsertAction& action);

    nlohmann::json CreateJson() const;

    friend std::ostream& operator<<(std::ostream& stream, const Solution& obj);

    size_t CalculateHash() const override;

    friend bool operator==(const Solution& sol1, const Solution& sol2);

private:
    static std::vector<Location> FindRealPath(const Location& location1, const Location& location2, bool realDistances);



};



#endif //ROUTING_PROBLEM_SOLUTION_H
