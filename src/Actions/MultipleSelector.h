//
// Created by Максим Процик on 01.05.2023.
//

#ifndef ROUTING_PROBLEM_MULTIPLESELECTOR_H
#define ROUTING_PROBLEM_MULTIPLESELECTOR_H

#include "Heuristic.h"

#include <vector>

class MultipleSelector: public Heuristic  {
public:
    MultipleSelector(std::vector<std::shared_ptr<Heuristic>> heuristics);
    std::vector<std::pair<std::shared_ptr<Action>, std::shared_ptr<Action>>> GetNeighboursActions(
            const Solution& solution, const std::unordered_set<ObjectId>& forbiddenIds
            ) override;
    std::vector<std::pair<std::shared_ptr<Action>, std::shared_ptr<Action>>> GetAllActions(const Solution& solution) override;
protected:
    std::vector<std::shared_ptr<Heuristic>> m_heuristics;
};


#endif //ROUTING_PROBLEM_MULTIPLESELECTOR_H
