//
// Created by Максим Процик on 01.05.2023.
//

#ifndef ROUTING_PROBLEM_REMOVELOOPS_H
#define ROUTING_PROBLEM_REMOVELOOPS_H


#include "ReverseAction.h"
#include "Heuristic.h"

class RemoveLoops: public Heuristic {
public:
    std::vector<std::pair<std::shared_ptr<Action>, std::shared_ptr<Action>>> GetNeighboursActions(
            const Solution& solution, const std::unordered_set<ObjectId>& forbiddenIds
            ) override;

};


#endif //ROUTING_PROBLEM_REMOVELOOPS_H
