//
// Created by Максим Процик on 01.05.2023.
//

#include "ReverseAction.h"

bool ReverseAction::PerformAction(Solution& sol) const
{
    if (start >= end)
    {
        return false;
    }
    if (route_index >= sol.Routes.size())
    {
        return false;
    }
    if (exportation_index >= sol.Routes[route_index].Exportations.size())
    {
        return false;
    }

    if (end >= sol.Routes[route_index].Exportations[exportation_index].Containers.size())
    {
        return false;
    }

    size_t first = start;
    size_t second = end;
    while (first < second)
    {
        std::swap(sol.Routes[route_index].Exportations[exportation_index].Containers[first],
                  sol.Routes[route_index].Exportations[exportation_index].Containers[second]);
        first++;
        second--;
    }

    if (Recalculate(sol))
    {
        sol.InvalidateHash();
        return true;
    }

    first = start;
    second = end;
    while (first < second)
    {
        std::swap(sol.Routes[route_index].Exportations[exportation_index].Containers[first],
                  sol.Routes[route_index].Exportations[exportation_index].Containers[second]);
        first++;
        second--;
    }
    Recalculate(sol);
    return false;

}

std::set<ObjectId> ReverseAction::GetAffectedIds(const Solution& sol) const
{
    std::set<ObjectId> objects;
    for (size_t i = start; i <= end; i++)
    {
        objects.insert(sol.Routes[route_index].Exportations[exportation_index].Containers[i]);
    }
    return objects;
}

bool ReverseAction::Recalculate(Solution& sol) const
{
    if (!sol.Routes[route_index].Exportations[exportation_index].RecalculateInnerValues(start) ||
        !sol.Routes[route_index].Exportations[exportation_index].RecalculateFinishValues())
    {
        return false;
    }

    for (size_t i = exportation_index + 1; i < sol.Routes[route_index].Exportations.size(); i++)
    {
        sol.Routes[route_index].Exportations[i].StartDistance = sol.Routes[route_index].Exportations[i-1].GetFinishDistance();
        sol.Routes[route_index].Exportations[i].StartTime = sol.Routes[route_index].Exportations[i-1].GetFinishTime();
        if (!sol.Routes[route_index].Exportations[i].RecalculateInnerValues(0) ||
            !sol.Routes[route_index].Exportations[i].RecalculateFinishValues())
        {
            return false;
        }
    }

    if (!sol.Routes[route_index].CalculateFinishValues())
    {
        return false;
    }

    return true;
}

double ReverseAction::ExpectedDiff(const Solution &sol) const
{
    Problem& problem = Problem::GetProblem();
    const Exportation& exportation = sol.Routes[route_index].Exportations[exportation_index];

    Location prevLoc;
    if (start == 0)
    {
        prevLoc = exportation.StartLocation;
    }
    else
    {
        ObjectId prevContainer = exportation.Containers[start-1];
        prevLoc = problem.GetContainer(prevContainer).GetLocation();
    }

    Location nextLoc;
    if (end + 1 == exportation.Containers.size())
    {
        nextLoc = exportation.GetFinishLocation();
    }
    else
    {
        ObjectId nextContainer = exportation.Containers[end+1];
        nextLoc = problem.GetContainer(nextContainer).GetLocation();
    }
    std::vector<Location> otherLocation;
    otherLocation.reserve(end - start + 1);
    for (size_t i = start; i <= end; i++)
    {
        ObjectId container = exportation.Containers[i];
        otherLocation.push_back(problem.GetContainer(container).GetLocation());
    }

    double originalDistance = 0;
    originalDistance += prevLoc.Distance(otherLocation.front()) + otherLocation.back().Distance(nextLoc);
    for (int i = 1; i < otherLocation.size(); i++)
    {
        originalDistance += otherLocation[i-1].Distance(otherLocation[i]);
    }

    double newDistance = 0;
    newDistance += prevLoc.Distance(otherLocation.back()) + otherLocation.front().Distance(nextLoc);
    for (int i = (int)(otherLocation.size()) - 2; i >= 0; i--)
    {
        newDistance += otherLocation[i+1].Distance(otherLocation[i]);
    }
    const GarbageTruck& truck = problem.GetTruck(sol.Routes[route_index].Truck);
    return (originalDistance - newDistance) * truck.GetFuelConsumption();
}
