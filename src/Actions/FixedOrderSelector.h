//
// Created by Максим Процик on 01.05.2023.
//

#ifndef ROUTING_PROBLEM_FIXEDORDERSELECTOR_H
#define ROUTING_PROBLEM_FIXEDORDERSELECTOR_H


#include "Heuristic.h"

class FixedOrderSelector: public Heuristic  {
public:
    FixedOrderSelector(std::vector<std::shared_ptr<Heuristic>> _heuristics, std::vector<size_t> _repeats);
    std::vector<std::pair<std::shared_ptr<Action>, std::shared_ptr<Action>>> GetNeighboursActions(
            const Solution& solution, const std::unordered_set<ObjectId>& forbiddenIds
            ) override;
    std::vector<std::pair<std::shared_ptr<Action>, std::shared_ptr<Action>>> GetNeighboursActions(const Solution& solution);
    std::vector<std::pair<std::shared_ptr<Action>, std::shared_ptr<Action>>> GetAllActions(const Solution& solution) override;
protected:
    std::vector<std::shared_ptr<Heuristic>> m_heuristics;
    std::vector<size_t> m_repeats;
    size_t m_heuristic_index = 0;
    size_t m_current_repeat = 0;
};


#endif //ROUTING_PROBLEM_FIXEDORDERSELECTOR_H
