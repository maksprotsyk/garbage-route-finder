//
// Created by Максим Процик on 01.05.2023.
//

#ifndef ROUTING_PROBLEM_SWAPANY_H
#define ROUTING_PROBLEM_SWAPANY_H


#include "InterChangeAction.h"
#include "Heuristic.h"
#include <random>

class SwapAny: public Heuristic {
public:
    size_t to_generate = 5;
    std::vector<std::pair<std::shared_ptr<Action>, std::shared_ptr<Action>>> GetNeighboursActions(
            const Solution& solution, const std::unordered_set<ObjectId>& forbiddenIds
            ) override;
protected:

    struct ContainerLocation
    {
        size_t routeIndex;
        size_t expIndex;
        size_t index;
        ObjectId id;
    };

    std::random_device rd;

};


#endif //ROUTING_PROBLEM_SWAPANY_H
