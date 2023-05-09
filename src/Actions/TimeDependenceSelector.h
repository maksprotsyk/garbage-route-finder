//
// Created by Максим Процик on 01.05.2023.
//

#ifndef ROUTING_PROBLEM_TIMEDEPENDENCESELECTOR_H
#define ROUTING_PROBLEM_TIMEDEPENDENCESELECTOR_H

#include "Heuristic.h"
#include <vector>


class TimeDependentSelector: public Heuristic  {
public:
    TimeDependentSelector(std::vector<std::shared_ptr<Heuristic>> heuristics, std::vector<size_t> repeats);
    std::vector<std::pair<std::shared_ptr<Action>, std::shared_ptr<Action>>> GetNeighboursActions(
            const Solution& solution, const std::unordered_set<ObjectId>& forbiddenIds
            ) override;
    std::vector<std::pair<std::shared_ptr<Action>, std::shared_ptr<Action>>> GetAllActions(const Solution& solution) override;
protected:
    std::vector<std::shared_ptr<Heuristic>> m_heuristics;
    std::vector<size_t> m_repeats;
    size_t m_heuristic_index = 0;
    size_t m_current_repeat = 0;
};



#endif //ROUTING_PROBLEM_TIMEDEPENDENCESELECTOR_H
