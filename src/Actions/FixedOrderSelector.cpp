//
// Created by Maksym Protsyk on 12/14/21.
//

#include "FixedOrderSelector.h"

FixedOrderSelector::FixedOrderSelector(
        std::vector<std::shared_ptr<Heuristic>> heuristics, std::vector<size_t> repeats
): m_heuristics(heuristics), m_repeats(repeats) {

    if (repeats.size() != heuristics.size()) {
        throw std::runtime_error("Repeats vector should be of the same size as heuristics vector");
    }
    for (unsigned long repeat_num : repeats) {
        if (repeat_num == 0) {
            throw std::runtime_error("Heuristic can't have 0 repeats");
        }
    }
}

std::vector<std::pair<std::shared_ptr<Action>, std::shared_ptr<Action>>> FixedOrderSelector::GetNeighboursActions(const Solution &solution, const std::unordered_set<ObjectId>& forbiddenIds) {
    auto actions = m_heuristics[m_heuristic_index]->GetNeighboursActions(solution, forbiddenIds);
    m_current_repeat++;
    if (m_current_repeat == m_repeats[m_heuristic_index]) {
        m_heuristic_index++;
        m_current_repeat = 0;
    }
    if (m_heuristic_index == m_heuristics.size()) {
        m_heuristic_index = 0;
    }
    return actions;
}

std::vector<std::pair<std::shared_ptr<Action>, std::shared_ptr<Action>>> FixedOrderSelector::GetAllActions(const Solution& solution)
{
    std::vector<std::pair<std::shared_ptr<Action>, std::shared_ptr<Action>>> actions;
    for (auto& heuristic: m_heuristics)
    {
        auto new_actions = heuristic->GetAllActions(solution);
        actions.insert(actions.end(), new_actions.begin(), new_actions.end());
    }
    return actions;
}
