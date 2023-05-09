//
// Created by Максим Процик on 01.05.2023.
//

#ifndef ROUTING_PROBLEM_ANNEALINGSEARCH_H
#define ROUTING_PROBLEM_ANNEALINGSEARCH_H

#include <queue>
#include "Solution.h"
#include "Heuristic.h"
#include "Search.h"

class AnnealingSearch: public Search {
public:
    size_t iterations;
    double tempCoef;
    size_t resets;

    Solution search(const Solution& sol) override;

private:
    double maxTemperature;
    double minTemperature;
    double alpha;
    double gamma;

    static double calculateProbability(double score1, double score2, double t);
    double calculateTemperature(double currentTemperature, size_t iteration) const;

};



#endif //ROUTING_PROBLEM_ANNEALINGSEARCH_H
