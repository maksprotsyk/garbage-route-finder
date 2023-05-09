//
// Created by Максим Процик on 01.05.2023.
//

#include "TabuSearch.h"
#include <unordered_set>

Solution TabuSearch::search(const Solution &sol)
{
    size_t queueSize = (size_t) (Problem::GetProblem().GetContainersNum() * queueCoef);

    Solution bestSolution = sol;

    double bestScore = heuristic->Score(bestSolution);

    Solution currentSolution = sol;

    std::deque<ObjectId> tabuQueue;
    std::unordered_set<ObjectId> forbiddenIds;
    for (size_t i = 0; i < iterations; i++) {
        double currentBestScore = -std::numeric_limits<double>::infinity();
        std::shared_ptr<Action> currentBestAction = nullptr;

        auto actions = heuristic->GetNeighboursActions(currentSolution, forbiddenIds);

        std::vector<std::thread> threads;
        for (size_t j = 1; j < threadsNum; j++)
        {
            threads.emplace_back(
                    &TabuSearch::SearchThread,
                    this, j,
                    std::cref(actions),
                    std::cref(currentSolution),
                    std::ref(currentBestScore),
                    std::ref(currentBestAction)
                    );
        }

        SearchThread(0, actions, currentSolution, currentBestScore, currentBestAction);

        for (auto& item: threads)
        {
            item.join();
        }

        if (!currentBestAction)
        {
            if (!tabuQueue.empty())
            {
                ObjectId id = tabuQueue.front();
                tabuQueue.pop_front();
                forbiddenIds.erase(id);
            }
            continue;
        }
        for (const auto& id: currentBestAction->GetAffectedIds(currentSolution))
        {
            tabuQueue.push_back(id);
            forbiddenIds.insert(id);
        }
        while (tabuQueue.size() > queueSize)
        {
            ObjectId id = tabuQueue.front();
            tabuQueue.pop_front();
            forbiddenIds.erase(id);
        }

        currentBestAction->PerformAction(currentSolution);

        if (currentBestScore > bestScore)
        {
            bestScore = currentBestScore;
            bestSolution = currentSolution;
        }

    }

    return bestSolution;

}

void TabuSearch::SearchThread(size_t index, const std::vector<std::pair<std::shared_ptr<Action>, std::shared_ptr<Action>>> &actions,
                              const Solution &currentSolution, double &bestScore, std::shared_ptr<Action> &bestAction)
{

    double epsilon = Arguments::GetArguments().GetValue<double>(Arguments::Epsilon);
    double threadBestScore = -std::numeric_limits<double>::infinity();
    std::shared_ptr<Action> threadBestAction = nullptr;

    Solution sol = currentSolution;
    double currentScore = heuristic->Score(sol);
    for (size_t i = index; i < actions.size(); i += threadsNum)
    {
        const auto& action = actions[i];
        double expectedDiff = action.first->ExpectedDiff(sol);
        if (std::abs(expectedDiff) < epsilon)
        {
            continue;
        }
        double newScore = currentScore + expectedDiff;
        if (newScore < threadBestScore)
        {
            continue;
        }
        if (!action.first->PerformAction(sol))
        {
            continue;
        }

        threadBestScore = newScore;
        threadBestAction = action.first;

        action.second->PerformAction(sol);
    }

    mutex.lock();
    if (threadBestScore > bestScore)
    {
        bestScore = threadBestScore;
        bestAction = threadBestAction;
    }
    mutex.unlock();
}
