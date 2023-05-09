//
// Created by Максим Процик on 01.05.2023.
//

#include "ClusterSolution.h"

Solution ClusterSolution::CreateSolution() const
{
    Solution sol;
    sol.Description += "INIT SOLUTION: cluster solution";

    Problem &problem = Problem::GetProblem();
    std::set<size_t> goodRoutes;

    for (ObjectId id = 0; id < problem.GetTrucksNum(); id++)
    {
        sol.Routes.emplace_back();
        sol.Routes.back().Truck = id;
        sol.UnusedTrucks.erase(id);
        goodRoutes.insert(id);
    }

    std::vector<int> clusters = HierarchicalAgglomerativeClustering(clusterCoef);

    while (!goodRoutes.empty())
    {
        for (auto itr = goodRoutes.begin(); itr != goodRoutes.end();)
        {

            Route &route = sol.Routes[*itr];
            route.Exportations.emplace_back();
            Exportation& lastExportation = route.Exportations.back();

            const auto& truck = problem.GetTruck(route.Truck);

            lastExportation.Truck = route.Truck;

            int lastCluster;

            if (route.Exportations.size() > 1)
            {
                lastExportation.StartLocation = route.Exportations[route.Exportations.size() - 2].GetFinishLocation();
                lastExportation.StartTime = route.Exportations[route.Exportations.size() - 2].GetFinishTime();
                lastExportation.StartDistance = route.Exportations[route.Exportations.size() - 2].GetFinishDistance();
                lastCluster = clusters[
                        problem.GetTrucksNum()
                        + problem.GetContainersNum()
                        + route.Exportations[route.Exportations.size() - 2].CurrentLandfill
                        ];
            }
            else
            {
                lastExportation.StartLocation = truck.GetLocation();
                lastExportation.StartTime = truck.GetTimeWindow().GetStart();
                lastExportation.StartDistance = 0;
                lastCluster = clusters[problem.GetContainersNum() + route.Truck];
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
                    lastCluster = clusters[lastExportation.Containers.back()];
                }

                std::vector<ObjectId> sortedLandfills;
                sortedLandfills.reserve(problem.GetLandfillsNum());
                for (ObjectId id = 0; id < problem.GetLandfillsNum(); id++)
                {
                    sortedLandfills.push_back(id);
                }

                std::sort(sortedLandfills.begin(), sortedLandfills.end(),
                          [&problem, &lastLocation, &lastCluster, &clusters](ObjectId a, ObjectId b)
                          {
                              int clusterA = clusters[problem.GetContainersNum() + problem.GetTrucksNum() + a];
                              int clusterB = clusters[problem.GetContainersNum() + problem.GetTrucksNum() + b];
                              if (lastCluster == clusterA && lastCluster != clusterB)
                              {
                                  return true;
                              }
                              else if (lastCluster == clusterB && lastCluster != clusterA)
                              {
                                  return false;
                              }
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
                          [&problem, &lastLocation, &lastCluster, &clusters](ObjectId a, ObjectId b)
                          {
                              int clusterA = clusters[a];
                              int clusterB = clusters[b];
                              if (lastCluster == clusterA && lastCluster != clusterB)
                              {
                                  return true;
                              }
                              else if (lastCluster == clusterB && lastCluster != clusterA)
                              {
                                  return false;
                              }

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


std::vector<int> ClusterSolution::HierarchicalAgglomerativeClustering(double coef)
{
    Problem& problem = Problem::GetProblem();
    size_t n = problem.GetLandfillsNum() + problem.GetContainersNum() + problem.GetTrucksNum();
    std::vector<std::vector<double>> clustersDistances(n, std::vector<double>(n, 0));
    std::vector<int> clusters(n, 0);

    std::vector<Location> locations;
    locations.reserve(n);

    for (ObjectId i = 0; i < problem.GetContainersNum(); i++)
    {
        locations.push_back(problem.GetContainer(i).GetLocation());
    }
    for (ObjectId i = 0; i < problem.GetTrucksNum(); i++)
    {
        locations.push_back(problem.GetTruck(i).GetLocation());
    }
    for (ObjectId i = 0; i < problem.GetLandfillsNum(); i++)
    {
        locations.push_back(problem.GetLandfill(i).GetLocation());
    }

    // at the start every point is in its own cluster
    for (int i = 0; i < n; i++)
    {
        clusters[i] = i;
    }

    double average = 0;
    // copying the matrix and calculating the sum
    for (size_t i = 0; i < n; i++)
    {
        for (size_t j = 0; j < n; j++)
        {
            clustersDistances[i][j] = locations[i].Distance(locations[j]);
            average += clustersDistances[i][j];
        }
    }

    // this value will be used to determine when we need to terminate our algorithm
    average /= n * n;

    while (n > 1)
    {
        // finding two closest clusters
        double min_val = std::numeric_limits<double>::infinity();
        size_t index1 = 0;
        size_t index2 = 0;

        for (size_t i = 0; i < n; i++)
        {
            for (size_t j = i + 1; j < n; j++)
            {
                if (min_val > clustersDistances[i][j])
                {
                    min_val = clustersDistances[i][j];
                    index1 = i;
                    index2 = j;

                }
            }
        }


        // terminating
        if (min_val > coef * average)
        {
            break;
        }

        // changing the cluster labels based on the merged clusters
        for (int i = 0; i < clusters.size(); i++)
        {
            if (clusters[i] == index2)
            {
                clusters[i] = index1;
            }
            else if (clusters[i] > index2)
            {
                clusters[i]--;
            }
        }

        // creating new cluster distance matrix which has size smaller by 1
        std::vector<std::vector<double>> newClusterDistances(n-1, std::vector<double>(n-1, 0));
        for (size_t i = 0; i < n-1; i++)
        {
            for (size_t j = 0; j < n-1; j++)
            {
                size_t first = i;
                size_t second = j;
                if (first >= index2)
                {
                    first++;
                }
                if (second >= index2)
                {
                    second++;
                }

                if (first != index1 && second != index1)
                {
                    newClusterDistances[i][j] = clustersDistances[first][second];
                }
                else if (first == index1 && second == index1)
                {
                    newClusterDistances[i][j] = 0;
                }
                else if (first == index1)
                {
                    newClusterDistances[i][j] = std::max(
                            clustersDistances[index1][second],
                            clustersDistances[index2][second]
                    );
                }
                else
                {
                    newClusterDistances[i][j] = std::max(
                            clustersDistances[index1][first],
                            clustersDistances[index2][first]
                    );
                }

            }
        }
        n--;
        clustersDistances = newClusterDistances;
    }


    return clusters;

}