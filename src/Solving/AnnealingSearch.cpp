//
// Created by Максим Процик on 01.05.2023.
//

#include "AnnealingSearch.h"


#include <random>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include "Search.h"
#include "AnnealingSearch.h"

Solution AnnealingSearch::search(const Solution& sol) {

    std::knuth_b rand_engine;

    std::uniform_real_distribution<> uniform_zero_to_one(0.0, 1.0);

    Solution bestSolution = sol;
    double bestScore = heuristic->Score(bestSolution);
    double epsilon = Arguments::GetArguments().GetValue<double>(Arguments::Epsilon);
    Solution currentSolution = sol;

    minTemperature = std::numeric_limits<double>::infinity();
    maxTemperature = 0;
    auto actionsInit = heuristic->GetAllActions(currentSolution);
    size_t goodActions = 0;
    for (const auto& [action, reverseAction]: actionsInit)
    {
        if (action->PerformAction(currentSolution))
        {
            double newScore = heuristic->Score(currentSolution);
            double diff = std::abs(newScore - bestScore);
            reverseAction->PerformAction(currentSolution);
            if (diff < epsilon)
            {
                continue;
            }
            goodActions++;
            minTemperature = std::min(minTemperature, diff);
            maxTemperature = std::max(maxTemperature, diff);

        }
    }

    size_t containersNum = Problem::GetProblem().GetContainersNum();
    alpha = (double) tempCoef * containersNum * goodActions;
    gamma = (double) tempCoef * containersNum ;

    double curScore = bestScore;

    double currentTemperature = maxTemperature;
    double bestTemperature = maxTemperature;
    double resetTemperature = maxTemperature;

    int resetsLeft = (int) resets;
    size_t i = 0;
    while (resetsLeft > 0 && i < iterations)
    {
        auto actions = heuristic->GetNeighboursActions(currentSolution);
        if (actions.empty())
        {
            i++;
            continue;
        }

        std::shuffle(std::begin(actions), std::end(actions), rand_engine);

        bool foundAction = false;
        for (const auto& [action, reverseAction]: actions)
        {
            double expectedDiff = action->ExpectedDiff(currentSolution);
            if (std::abs(expectedDiff) < epsilon)
            {
                continue;
            }
            double newScore = curScore + expectedDiff;
            double p = calculateProbability(curScore, newScore, currentTemperature);
            if (uniform_zero_to_one(rand_engine) >= p)
            {
                continue;
            }
            if (!action->PerformAction(currentSolution))
            {
                continue;
            }

            if (newScore > bestScore)
            {
                bestSolution = currentSolution;
                bestScore = newScore;
                bestTemperature = currentTemperature;
                resetsLeft = (int)resets;
            }
            foundAction = true;
            curScore = newScore;

            break;
        }

        if (!foundAction)
        {
            resetTemperature = std::max(resetTemperature / 2, bestTemperature);
            currentTemperature = resetTemperature;
            resetsLeft--;
        }
        else
        {
            currentTemperature = calculateTemperature(currentTemperature, i);
        }
        i++;
    }

    return bestSolution;
}

double AnnealingSearch::calculateProbability(double score1, double score2, double t)
{
    if (score2 > score1)
    {
        return 1.0;
    }
    double relation = std::abs((score1 - score2));
    double prob = exp(- relation / t);
    return prob;
}

double AnnealingSearch::calculateTemperature(double currentTemperature, size_t iteration) const
{
    double beta = (maxTemperature - minTemperature) / ((alpha + gamma * std::sqrt((double)iteration)) * maxTemperature * minTemperature);
    double newTemperature = currentTemperature / (1 + beta * currentTemperature);
    return newTemperature;
}
