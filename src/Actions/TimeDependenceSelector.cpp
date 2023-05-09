//
// Created by Максим Процик on 01.05.2023.
//

#include "TimeDependenceSelector.h"


TimeDependentSelector::TimeDependentSelector(
        std::vector<std::shared_ptr<Heuristic>> heuristics, std::vector<size_t> repeats
): m_heuristics(heuristics), m_repeats(repeats) {

    if (repeats.size() + 1!= heuristics.size()) {
        throw std::runtime_error("Repeats vector should be 1 item size less than the heuristics vector");
    }
    for (unsigned long repeat_num : repeats) {
        if (repeat_num == 0) {
            throw std::runtime_error("Heuristic can't have 0 repeats");
        }
    }
}

std::vector<std::pair<std::shared_ptr<Action>, std::shared_ptr<Action>>> TimeDependentSelector::GetNeighboursActions(
        const Solution &solution, const std::unordered_set<ObjectId>& forbiddenIds
        ) {
    auto actions = m_heuristics[m_heuristic_index]->GetNeighboursActions(solution, forbiddenIds);
    if (m_heuristic_index == m_repeats.size()) {
        return actions;
    }

    m_current_repeat++;

    if (m_current_repeat == m_repeats[m_heuristic_index]) {
        m_heuristic_index++;
        m_current_repeat= 0;
    }
    return actions;
}

std::vector<std::pair<std::shared_ptr<Action>, std::shared_ptr<Action>>> TimeDependentSelector::GetAllActions(const Solution& solution)
{
    std::vector<std::pair<std::shared_ptr<Action>, std::shared_ptr<Action>>> actions;
    for (auto& heuristic: m_heuristics)
    {
        auto new_actions = heuristic->GetAllActions(solution);
        actions.insert(actions.end(), new_actions.begin(), new_actions.end());
    }
    return actions;
}
