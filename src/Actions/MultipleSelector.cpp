//
// Created by Максим Процик on 01.05.2023.
//

#include "MultipleSelector.h"

std::vector<std::pair<std::shared_ptr<Action>, std::shared_ptr<Action>>>
MultipleSelector::GetNeighboursActions(const Solution &solution, const std::unordered_set<ObjectId>& forbiddenIds)
{
    std::vector<std::pair<std::shared_ptr<Action>, std::shared_ptr<Action>>> actions;
    for (auto& heuristic: m_heuristics)
    {
        auto new_actions = heuristic->GetNeighboursActions(solution, forbiddenIds);
        actions.insert(actions.end(), new_actions.begin(), new_actions.end());
    }
    return actions;
}

MultipleSelector::MultipleSelector(std::vector<std::shared_ptr<Heuristic>> heuristics): m_heuristics(heuristics)
{

}

std::vector<std::pair<std::shared_ptr<Action>, std::shared_ptr<Action>>> MultipleSelector::GetAllActions(const Solution& solution)
{
    std::vector<std::pair<std::shared_ptr<Action>, std::shared_ptr<Action>>> actions;
    for (auto& heuristic: m_heuristics)
    {
        auto new_actions = heuristic->GetAllActions(solution);
        actions.insert(actions.end(), new_actions.begin(), new_actions.end());
    }
    return actions;
}
