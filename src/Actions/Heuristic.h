//
// Created by Максим Процик on 01.05.2023.
//

#ifndef ROUTING_PROBLEM_HEURISTIC_H
#define ROUTING_PROBLEM_HEURISTIC_H

#include "Solution.h"
#include "Action.h"
#include <unordered_set>

class Heuristic {
public:
    virtual std::vector<std::pair<std::shared_ptr<Action>, std::shared_ptr<Action>>> GetNeighboursActions(const Solution& solution, const std::unordered_set<ObjectId>& forbiddenIds) = 0;
    std::vector<std::pair<std::shared_ptr<Action>, std::shared_ptr<Action>>> GetNeighboursActions(const Solution& solution);
    virtual std::vector<std::pair<std::shared_ptr<Action>, std::shared_ptr<Action>>> GetAllActions(const Solution& solution);
    virtual double Score(const Solution& solution);
    virtual ~Heuristic() = default;
};

#endif //ROUTING_PROBLEM_HEURISTIC_H
