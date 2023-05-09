//
// Created by Максим Процик on 01.05.2023.
//

#ifndef ROUTING_PROBLEM_SWAPNEIGHBOURS_H
#define ROUTING_PROBLEM_SWAPNEIGHBOURS_H


#include "Heuristic.h"
#include "InterChangeAction.h"

class SwapNeighbours: public Heuristic {
public:
    std::vector<std::pair<std::shared_ptr<Action>, std::shared_ptr<Action>>> GetNeighboursActions(
            const Solution& solution, const std::unordered_set<ObjectId>& forbiddenIds
            ) override;

};


#endif //ROUTING_PROBLEM_SWAPNEIGHBOURS_H
