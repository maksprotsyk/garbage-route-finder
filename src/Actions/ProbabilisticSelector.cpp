//
// Created by Максим Процик on 01.05.2023.
//

#include "ProbabilisticSelector.h"

std::random_device ProbabilisticSelector::rd;

ProbabilisticSelector::ProbabilisticSelector(
        std::vector<std::shared_ptr<Heuristic>> heuristics, std::vector<double> weights
): m_distribution(weights.begin(), weights.end()), m_heuristics(heuristics){
    if (weights.size() != heuristics.size()) {
        throw std::runtime_error("Weight vector size should be equal to heuristics vector size");
    }

}
std::vector<std::pair<std::shared_ptr<Action>, std::shared_ptr<Action>>> ProbabilisticSelector::GetNeighboursActions(
        const Solution& solution, const std::unordered_set<ObjectId>& forbiddenIds
        ){
    size_t index = m_distribution(rd);
    return m_heuristics[index]->GetNeighboursActions(solution, forbiddenIds);
}

std::vector<std::pair<std::shared_ptr<Action>, std::shared_ptr<Action>>> ProbabilisticSelector::GetAllActions(const Solution& solution)
{
    std::vector<std::pair<std::shared_ptr<Action>, std::shared_ptr<Action>>> actions;
    for (auto& heuristic: m_heuristics)
    {
        auto new_actions = heuristic->GetAllActions(solution);
        actions.insert(actions.end(), new_actions.begin(), new_actions.end());
    }
    return actions;
}