//
// Created by Максим Процик on 30.04.2023.
//

#include "GreedySolution.h"

Solution GreedySolution::CreateSolution() const
{
    Solution sol;
    sol.Description += "INIT SOLUTION: simple_greed_solution";

    Problem &problem = Problem::GetProblem();
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
                Location lastLocation;
                if (lastExportation.Containers.empty())
                {
                    lastLocation = lastExportation.StartLocation;
                }
                else
                {
                    const auto& lastContainer = problem.GetContainer(lastExportation.Containers.back());
                    lastLocation = lastContainer.GetLocation();
                }

                std::vector<ObjectId> sortedLandfills;
                sortedLandfills.reserve(problem.GetLandfillsNum());
                for (ObjectId id = 0; id < problem.GetLandfillsNum(); id++)
                {
                    sortedLandfills.push_back(id);
                }

                std::sort(sortedLandfills.begin(), sortedLandfills.end(),
                          [&problem, &lastLocation](ObjectId a, ObjectId b)
                          {
                              const Landfill& first = problem.GetLandfill(a);
                              const Landfill& second = problem.GetLandfill(b);
                              return lastLocation.Distance(first.GetLocation())
                                     < lastLocation.Distance(second.GetLocation());
                          });

                std::vector<ObjectId> sortedContainers;
                sortedContainers.reserve(sol.Orphans.size());
                for (ObjectId id: sol.Orphans)
                {
                    sortedContainers.push_back(id);
                }

                std::sort(sortedContainers.begin(), sortedContainers.end(),
                          [&problem, &lastLocation](ObjectId a, ObjectId b)
                          {
                              const Container& first = problem.GetContainer(a);
                              const Container& second = problem.GetContainer(b);
                              return lastLocation.Distance(first.GetLocation())
                                     < lastLocation.Distance(second.GetLocation());
                          });

                bool found = false;

                ObjectId oldLandfill = lastExportation.CurrentLandfill;
                for (ObjectId container: sortedContainers)
                {
                    for (ObjectId landfill: sortedLandfills)
                    {
                        lastExportation.Containers.push_back(container);
                        lastExportation.CurrentLandfill = landfill;
                        if (lastExportation.RecalculateInnerValues(lastExportation.Containers.size() - 1) &&
                            lastExportation.RecalculateFinishValues() &&
                            route.CalculateFinishValues())
                        {
                            found = true;
                            sol.Orphans.erase(container);
                            break;
                        }
                        else
                        {
                            lastExportation.Containers.pop_back();
                            lastExportation.CurrentLandfill = oldLandfill;
                        }
                    }
                    if (found)
                    {
                        break;
                    }
                }

                if (!found)
                {
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
    return sol;

}

