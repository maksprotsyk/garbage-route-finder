//
// Created by Максим Процик on 01.05.2023.
//

#include "Heuristic.h"

double Heuristic::Score(const Solution& solution)
{
    return -solution.CalculateCost();
}

std::vector<std::pair<std::shared_ptr<Action>, std::shared_ptr<Action>>> Heuristic::GetNeighboursActions(const Solution& solution)
{
    return GetNeighboursActions(solution, {});
}

std::vector<std::pair<std::shared_ptr<Action>, std::shared_ptr<Action>>> Heuristic::GetAllActions(const Solution& solution)
{
    return GetNeighboursActions(solution);
}