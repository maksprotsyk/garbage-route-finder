//
// Created by Максим Процик on 01.05.2023.
//

#ifndef ROUTING_PROBLEM_TABUSEARCH_H
#define ROUTING_PROBLEM_TABUSEARCH_H

#include "Search.h"
#include <thread>
#include <mutex>

class TabuSearch: public Search
{
public:
    double queueCoef;
    size_t iterations;
    size_t threadsNum;
    Solution search(const Solution& sol) override;
protected:
    std::mutex mutex;

    void SearchThread(
            size_t index,
            const std::vector<std::pair<std::shared_ptr<Action>, std::shared_ptr<Action>>>& actions,
            const Solution& currentSolution,
            double& bestScore, std::shared_ptr<Action>& bestAction);
};


#endif //ROUTING_PROBLEM_TABUSEARCH_H
