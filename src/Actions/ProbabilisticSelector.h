//
// Created by Максим Процик on 01.05.2023.
//

#ifndef ROUTING_PROBLEM_PROBABILISTICSELECTOR_H
#define ROUTING_PROBLEM_PROBABILISTICSELECTOR_H

#include "Heuristic.h"

#include <random>

class ProbabilisticSelector: public Heuristic  {
public:

    ProbabilisticSelector(std::vector<std::shared_ptr<Heuristic>> heuristics, std::vector<double> weights);
    std::vector<std::pair<std::shared_ptr<Action>, std::shared_ptr<Action>>> GetNeighboursActions(
            const Solution& solution, const std::unordered_set<ObjectId>& forbiddenIds
            ) override;
    std::vector<std::pair<std::shared_ptr<Action>, std::shared_ptr<Action>>> GetAllActions(const Solution& solution) override;
private:
    static std::random_device rd;
    std::vector<std::shared_ptr<Heuristic>> m_heuristics;
    std::discrete_distribution<double> m_distribution;
};



#endif //ROUTING_PROBLEM_PROBABILISTICSELECTOR_H
