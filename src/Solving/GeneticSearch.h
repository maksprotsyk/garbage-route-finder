//
// Created by Максим Процик on 01.05.2023.
//

#ifndef ROUTING_PROBLEM_GENETICSEARCH_H
#define ROUTING_PROBLEM_GENETICSEARCH_H

#include "Solution.h"
#include "Heuristic.h"
#include "Search.h"
#include "EmptyAction.h"

#include <random>
#include <thread>

class GeneticSearch: public Search {
public:
    size_t iterations;
    size_t populationSize;
    double mutationProbability;
    size_t threadsNum;

    Solution search(const Solution& sol) override;

private:
    std::knuth_b rand_engine;
    std::mutex m;

    std::shared_ptr<Solution> CreateChild(const Solution& parent1, const Solution& parent2);
    void PerformRandomAction(Solution& originalSolution);
    bool PerformRandomGoodAction(Solution& originalSolution);
    std::pair<size_t, size_t> GenerateParentIndices(const std::vector<double>& probabilities);
    static std::vector<ObjectId> GetContainersOrder(const Solution& sol);
    static std::shared_ptr<Solution> CreateSolutionFromOrder(const std::vector<ObjectId>& order);
    static std::vector<double> CalculateProbabilities(const std::vector<double>& scores, double bestScore);

    void GeneticThread(
            size_t index, const std::vector<std::shared_ptr<Solution>>& population,
            const std::vector<double>& probs, std::vector<std::shared_ptr<Solution>>& newPopulation
            );
};



#endif //ROUTING_PROBLEM_GENETICSEARCH_H
