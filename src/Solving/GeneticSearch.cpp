
#include <cstdlib>
#include <ctime>
#include <iostream>

#include "GeneticSearch.h"

Solution GeneticSearch::search(const Solution& sol)
{
    Solution bestSolution = sol;
    double bestScore = heuristic->Score(sol);

    std::vector<std::shared_ptr<Solution>> currentPopulation;
    std::vector<double> scores;
    currentPopulation.reserve(populationSize);
    scores.reserve(populationSize);
    for (size_t i = 0; i < populationSize; i++)
    {
        currentPopulation.push_back(std::make_shared<Solution>(bestSolution));
        scores.push_back(bestScore);
    }

    for (size_t i = 0; i < iterations; i++)
    {

        std::vector<double> probabilities = CalculateProbabilities(scores, bestScore);

        std::vector<std::shared_ptr<Solution>> newCurrentPopulation;
        newCurrentPopulation.reserve(2 * populationSize + 1);
        newCurrentPopulation.push_back(std::make_shared<Solution>(bestSolution));

        std::vector<std::thread> threads;
        for (size_t j = 1; j < threadsNum; j++)
        {
            threads.emplace_back(
                    &GeneticSearch::GeneticThread,
                    this,
                    j,
                    std::cref(currentPopulation),
                    std::cref(probabilities),
                    std::ref(newCurrentPopulation)
                    );
        }

        GeneticThread(0, currentPopulation, probabilities, newCurrentPopulation);

        for (auto& t: threads)
        {
            t.join();
        }

        std::vector<double> newScores;
        for (auto& item: newCurrentPopulation)
        {
            newScores.emplace_back(heuristic->Score(*item));
        }

        std::sort(newCurrentPopulation.begin(), newCurrentPopulation.end(),
                  [this](const std::shared_ptr<Solution>& a, const std::shared_ptr<Solution>& b)
                  {
                      return heuristic->Score(*a) > heuristic->Score(*b);
                  });

        int shuffleIndex = (int) (populationSize + 1) / 2;
        std::shuffle(newCurrentPopulation.begin() + shuffleIndex, newCurrentPopulation.end(), rand_engine);

        while (newCurrentPopulation.size() > populationSize)
        {
            newCurrentPopulation.pop_back();
        }
        scores.clear();
        for (const auto& solution: newCurrentPopulation)
        {
            scores.push_back(heuristic->Score(*solution));
        }
        currentPopulation = std::move(newCurrentPopulation);
        if (scores[0] > bestScore)
        {
            bestSolution = *currentPopulation[0];
            bestScore = scores[0];
        }

    }


    return bestSolution;
}

std::shared_ptr<Solution> GeneticSearch::CreateChild(const Solution& parent1, const Solution& parent2)
{

    std::vector<ObjectId> order1 = GetContainersOrder(parent1);
    std::vector<ObjectId> order2 = GetContainersOrder(parent2);
    std::uniform_int_distribution<> indexRandomizer(0, (int)order1.size() - 1);
    size_t start = indexRandomizer(rand_engine);
    size_t end = indexRandomizer(rand_engine);
    if (start > end)
    {
        std::swap(start, end);
    }
    std::vector<ObjectId> newOrder;
    newOrder.reserve(order1.size());
    std::unordered_set<ObjectId> usedContainers;
    usedContainers.reserve(end - start);
    for (size_t i = start; i < end; i++)
    {
        usedContainers.insert(order1[i]);
    }

    size_t index = 0;
    for (size_t i = 0; i < start; i++)
    {
        while (usedContainers.find(order2[index]) != usedContainers.end())
        {
            index++;
        }
        newOrder.push_back(order2[index]);
        index++;
    }

    for (size_t i = start; i < end; i++)
    {
        newOrder.push_back(order1[i]);
    }

    for (size_t i = end; i < order1.size(); i++)
    {
        while (usedContainers.find(order2[index]) != usedContainers.end())
        {
            index++;
        }
        newOrder.push_back(order2[index]);
        index++;
    }

    return CreateSolutionFromOrder(newOrder);
}

void GeneticSearch::PerformRandomAction(Solution& originalSolution)
{
    double epsilon = Arguments::GetArguments().GetValue<double>(Arguments::Epsilon);
    auto actions = heuristic->GetNeighboursActions(originalSolution);
    std::shuffle(std::begin(actions), std::end(actions), rand_engine);
    for (const auto& action: actions)
    {
        if (std::abs(action.first->ExpectedDiff(originalSolution)) < epsilon)
        {
            continue;
        }
        if (action.first->PerformAction(originalSolution))
        {
            break;
        }
    }
}

bool GeneticSearch::PerformRandomGoodAction(Solution& originalSolution)
{
    double epsilon = Arguments::GetArguments().GetValue<double>(Arguments::Epsilon);
    auto actions = heuristic->GetNeighboursActions(originalSolution);
    std::shuffle(std::begin(actions), std::end(actions), rand_engine);
    for (const auto& action: actions)
    {
        if (std::abs(action.first->ExpectedDiff(originalSolution)) < epsilon)
        {
            continue;
        }
        if (action.first->ExpectedDiff(originalSolution) < 0)
        {
            continue;
        }
        if (action.first->PerformAction(originalSolution))
        {
            return true;
        }
    }
    return false;

}

std::pair<size_t, size_t> GeneticSearch::GenerateParentIndices(const std::vector<double>& scores)
{
    std::uniform_real_distribution<> uniform_zero_to_one(0.0, 1.0);
    double val = uniform_zero_to_one(rand_engine);
    size_t parent1 = 0;
    for (size_t scoreIndex = 0; scoreIndex < scores.size(); scoreIndex++)
    {
        if (scores[scoreIndex] >= val)
        {
            parent1 = scoreIndex;
            break;
        }
    }
    size_t parent2 = parent1;
    while (parent1 == parent2)
    {
        val = uniform_zero_to_one(rand_engine);
        for (size_t scoreIndex = 0; scoreIndex < scores.size(); scoreIndex++)
        {
            if (scores[scoreIndex] >= val)
            {
                parent2 = scoreIndex;
                break;
            }
        }
    }
    return std::pair<size_t, size_t>(parent1, parent2);
}

std::vector<ObjectId> GeneticSearch::GetContainersOrder(const Solution& sol)
{
    Problem& problem = Problem::GetProblem();

    size_t containersNum = problem.GetContainersNum();
    std::vector<ObjectId> containers;
    containers.reserve(containersNum);

    std::vector<size_t> goodRoutes;
    goodRoutes.reserve(sol.Routes.size());
    for (size_t i = 0; i < sol.Routes.size(); i++)
    {
        goodRoutes.push_back(i);
    }

    size_t exportationIndex = 0;
    while (!goodRoutes.empty())
    {
        for (auto itr = goodRoutes.begin(); itr != goodRoutes.end();)
        {
            if (sol.Routes[*itr].Exportations.size() <= exportationIndex)
            {
                itr = goodRoutes.erase(itr);
                continue;
            }
            for (ObjectId cont: sol.Routes[*itr].Exportations[exportationIndex].Containers)
            {
                containers.push_back(cont);
            }
            itr++;
        }
        exportationIndex++;
    }

    for (ObjectId cont: sol.Orphans)
    {
        containers.push_back(cont);
    }

    return containers;

}

std::shared_ptr<Solution> GeneticSearch::CreateSolutionFromOrder(const std::vector<ObjectId> &order)
{
    Problem& problem = Problem::GetProblem();
    size_t containerIndex = 0;
    std::shared_ptr<Solution> sol_ptr = std::make_shared<Solution>();
    Solution& sol = *sol_ptr;
    std::set<size_t> goodRoutes;

    for (ObjectId id = 0; id < problem.GetTrucksNum(); id++)
    {
        sol.Routes.emplace_back();
        sol.Routes.back().Truck = id;
        sol.UnusedTrucks.erase(id);
        goodRoutes.insert(id);
    }

    while (!goodRoutes.empty())
    {
        for (auto itr = goodRoutes.begin(); itr != goodRoutes.end();)
        {

            Route &route = sol.Routes[*itr];
            route.Exportations.emplace_back();
            Exportation& lastExportation = route.Exportations.back();

            const auto& truck = problem.GetTruck(route.Truck);

            lastExportation.Truck = route.Truck;
            if (route.Exportations.size() > 1)
            {
                lastExportation.StartLocation = route.Exportations[route.Exportations.size() - 2].GetFinishLocation();
                lastExportation.StartTime = route.Exportations[route.Exportations.size() - 2].GetFinishTime();
                lastExportation.StartDistance = route.Exportations[route.Exportations.size() - 2].GetFinishDistance();
            }
            else
            {
                lastExportation.StartLocation = truck.GetLocation();
                lastExportation.StartTime = truck.GetTimeWindow().GetStart();
                lastExportation.StartDistance = 0;
            }

            while (true)
            {

                bool added = false;
                if (containerIndex >= order.size())
                {
                    break;
                }

                std::vector<ObjectId> sortedLandfills;
                sortedLandfills.reserve(problem.GetLandfillsNum());
                for (ObjectId id = 0; id < problem.GetLandfillsNum(); id++)
                {
                    sortedLandfills.push_back(id);
                }

                ObjectId container = order[containerIndex];
                const Location& location = problem.GetContainer(container).GetLocation();

                std::sort(sortedLandfills.begin(), sortedLandfills.end(),
                          [&problem, &location](ObjectId a, ObjectId b)
                          {
                              const Landfill& first = problem.GetLandfill(a);
                              const Landfill& second = problem.GetLandfill(b);
                              return location.Distance(first.GetLocation())
                                     < location.Distance(second.GetLocation());
                          });


                ObjectId oldLandfill = lastExportation.CurrentLandfill;
                lastExportation.Containers.push_back(container);
                for (ObjectId landfill: sortedLandfills)
                {
                    lastExportation.CurrentLandfill = landfill;
                    if (lastExportation.RecalculateInnerValues(lastExportation.Containers.size() - 1) &&
                        lastExportation.RecalculateFinishValues() &&
                        route.CalculateFinishValues())
                    {
                        added = true;
                        sol.Orphans.erase(container);
                        containerIndex++;
                        break;
                    }
                }

                if (!added)
                {
                    lastExportation.Containers.pop_back();
                    lastExportation.CurrentLandfill = oldLandfill;
                    if (oldLandfill > 0)
                    {
                        lastExportation.RecalculateInnerValues(lastExportation.Containers.size() - 1);
                        lastExportation.RecalculateFinishValues();
                        route.CalculateFinishValues();
                    }

                    break;
                }

            }

            if (lastExportation.Containers.empty())
            {
                route.Exportations.pop_back();
                itr = goodRoutes.erase(itr);
            }
            else
            {
                itr++;
            }
        }

    }


    for (auto itr = sol.Routes.begin(); itr != sol.Routes.end();)
    {
        Route &route = *itr;
        if (route.Exportations.empty())
        {
            ObjectId truck = route.Truck;
            sol.UnusedTrucks.insert(truck);
            itr = sol.Routes.erase(itr);
        }
        else
        {
            itr++;
        }
    }
    sol.InvalidateHash();
    return sol_ptr;
}

std::vector<double> GeneticSearch::CalculateProbabilities(const std::vector<double> &scores, double bestScore)
{
    std::vector<double> probabilies;
    probabilies.reserve(scores.size());

    double scoresSum = 0;
    double addition = std::abs(*std::min_element(scores.begin(), scores.end()) + bestScore/100);

    for (const double& score : scores)
    {
        scoresSum += addition + score;
        probabilies.push_back(scoresSum);
    }

    for (double& prob : probabilies)
    {
        prob /= scoresSum;
    }
    return probabilies;
}

void GeneticSearch::GeneticThread(
        size_t index, const std::vector<std::shared_ptr<Solution>>& population,
        const std::vector<double>& probs, std::vector<std::shared_ptr<Solution>>& newPopulation)
{
    std::uniform_real_distribution<double> mutationRandomizer(0.0, 1.0);
    for (size_t i = index; i < populationSize; i += threadsNum )
    {
        auto [parent1, parent2] = GenerateParentIndices(probs);
        auto child1 = CreateChild(*population[parent1], *population[parent2]);
        double mutationProb1 = mutationRandomizer(rand_engine);
        if (mutationProb1 <= mutationProbability || !PerformRandomGoodAction(*child1))
        {
            PerformRandomAction(*child1);
        }

        auto child2 = CreateChild(*population[parent2], *population[parent1]);
        double mutationProb2 = mutationRandomizer(rand_engine);
        if (mutationProb2 <= mutationProbability || !PerformRandomGoodAction(*child2))
        {
            PerformRandomAction(*child2);
        }
        m.lock();
        newPopulation.push_back(child1);
        newPopulation.push_back(child2);
        m.unlock();
    }

}

