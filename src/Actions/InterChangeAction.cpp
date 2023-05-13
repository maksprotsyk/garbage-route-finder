//
// Created by Максим Процик on 01.05.2023.
//

#include "InterChangeAction.h"

bool InterChangeAction::PerformAction(Solution& sol) const
{
    if (route_index1 == route_index2 && exportation_index1 == exportation_index2 && container_index1 == container_index2)
    {
        return false;
    }

    if (sol.Routes.size() <= route_index1)
    {
        return false;
    }
    if (sol.Routes[route_index1].Exportations.size() <= exportation_index1)
    {
        return false;
    }
    if (sol.Routes[route_index1].Exportations[exportation_index1].Containers.size() <= container_index1)
    {
        return false;
    }
    ObjectId& first = sol.Routes[route_index1].Exportations[exportation_index1].Containers[container_index1];

    if (sol.Routes.size() <= route_index2)
    {
        return false;
    }

    if (sol.Routes[route_index2].Exportations.size() <= exportation_index2)
    {
        return false;
    }

    if (sol.Routes[route_index2].Exportations[exportation_index2].Containers.size() <= container_index2)
    {
        return false;
    }

    ObjectId& second = sol.Routes[route_index2].Exportations[exportation_index2].Containers[container_index2];

    std::swap(first, second);

    if (!Recalculate(sol))
    {
        std::swap(first, second);
        Recalculate(sol);
        return false;
    }
    sol.InvalidateHash();
    return true;
}

bool InterChangeAction::Recalculate(Solution& sol) const
{
    if (!sol.Routes[route_index1].Exportations[exportation_index1].RecalculateInnerValues(container_index1) ||
        !sol.Routes[route_index1].Exportations[exportation_index1].RecalculateFinishValues())
    {
        return false;
    }

    for (size_t i = exportation_index1 + 1; i < sol.Routes[route_index1].Exportations.size(); i++)
    {
        sol.Routes[route_index1].Exportations[i].StartDistance = sol.Routes[route_index1].Exportations[i-1].GetFinishDistance();
        sol.Routes[route_index1].Exportations[i].StartTime = sol.Routes[route_index1].Exportations[i-1].GetFinishTime();
        if (!sol.Routes[route_index1].Exportations[i].RecalculateInnerValues(0) ||
            !sol.Routes[route_index1].Exportations[i].RecalculateFinishValues())
        {
            return false;
        }
    }

    if (!sol.Routes[route_index1].CalculateFinishValues())
    {
        return false;
    }

    if (!sol.Routes[route_index2].Exportations[exportation_index2].RecalculateInnerValues(container_index2) ||
        !sol.Routes[route_index2].Exportations[exportation_index2].RecalculateFinishValues())
    {
        return false;
    }

    for (size_t i = exportation_index2 + 1; i < sol.Routes[route_index2].Exportations.size(); i++)
    {
        sol.Routes[route_index2].Exportations[i].StartDistance = sol.Routes[route_index2].Exportations[i-1].GetFinishDistance();
        sol.Routes[route_index2].Exportations[i].StartTime = sol.Routes[route_index2].Exportations[i-1].GetFinishTime();
        if (!sol.Routes[route_index2].Exportations[i].RecalculateInnerValues(0) ||
            !sol.Routes[route_index2].Exportations[i].RecalculateFinishValues())
        {
            return false;
        }
    }

    if (!sol.Routes[route_index2].CalculateFinishValues())
    {
        return false;
    }

    return true;
}

std::set<ObjectId> InterChangeAction::GetAffectedIds(const Solution& sol) const
{
    ObjectId first = sol.Routes[route_index1].Exportations[exportation_index1].Containers[container_index1];
    ObjectId second = sol.Routes[route_index2].Exportations[exportation_index2].Containers[container_index2];
    std::set<ObjectId> objects;
    objects.insert(first);
    objects.insert(second);
    return objects;
}

double InterChangeAction::ExpectedDiff(const Solution& sol) const
{
    Problem& problem = Problem::GetProblem();
    Location loc1;
    Location prevLoc1;
    Location nextLoc1;
    Location loc2;
    Location prevLoc2;
    Location nextLoc2;

    const Exportation& exportation1 = sol.Routes[route_index1].Exportations[exportation_index1];
    const Exportation& exportation2 = sol.Routes[route_index2].Exportations[exportation_index2];
    ObjectId container1 = exportation1.Containers[container_index1];
    ObjectId container2 = exportation2.Containers[container_index2];

    loc1 = problem.GetContainer(container1).GetLocation();
    loc2 = problem.GetContainer(container2).GetLocation();

    if (container_index1 == 0)
    {
        prevLoc1 = exportation1.StartLocation;
    }
    else
    {
        ObjectId prevContainer = exportation1.Containers[container_index1-1];
        prevLoc1 = problem.GetContainer(prevContainer).GetLocation();
    }

    if (container_index2 == 0)
    {
        prevLoc2 = exportation2.StartLocation;
    }
    else
    {
        ObjectId prevContainer = exportation2.Containers[container_index2-1];
        prevLoc2 = problem.GetContainer(prevContainer).GetLocation();
    }

    if (container_index1 + 1 == exportation1.Containers.size())
    {
        nextLoc1 = exportation1.GetFinishLocation();
    }
    else
    {
        ObjectId nextContainer = exportation1.Containers[container_index1+1];
        nextLoc1 = problem.GetContainer(nextContainer).GetLocation();
    }

    if (container_index2 + 1 == exportation2.Containers.size())
    {
        nextLoc2 = exportation2.GetFinishLocation();
    }
    else
    {
        ObjectId nextContainer = exportation2.Containers[container_index2+1];
        nextLoc2 = problem.GetContainer(nextContainer).GetLocation();
    }

    const GarbageTruck& truck1 = problem.GetTruck(sol.Routes[route_index1].Truck);
    const GarbageTruck& truck2 = problem.GetTruck(sol.Routes[route_index2].Truck);

    if (exportation_index1 == exportation_index2 && route_index1 == route_index2)
    {
        if (container_index2 == container_index1 + 1)
        {
            double oldDistance = prevLoc1.Distance(loc1) + loc1.Distance(loc2) + loc2.Distance(nextLoc2);
            double newDistance = prevLoc1.Distance(loc2) + loc2.Distance(loc1) + loc1.Distance(nextLoc2);
            return (oldDistance - newDistance) * truck1.GetFuelConsumption();
        }
        else if (container_index2 + 1 == container_index1)
        {
            double oldDistance = prevLoc2.Distance(loc2) + loc2.Distance(loc1) + loc1.Distance(nextLoc1);
            double newDistance = prevLoc2.Distance(loc1) + loc1.Distance(loc2) + loc2.Distance(nextLoc1);
            return (oldDistance - newDistance) * truck1.GetFuelConsumption();
        }

    }

    double currentFuel = (prevLoc1.Distance(loc1) + loc1.Distance(nextLoc1)) * truck1.GetFuelConsumption()
                         + (prevLoc2.Distance(loc2) + loc2.Distance(nextLoc2)) * truck2.GetFuelConsumption();


    double newFuel = (prevLoc1.Distance(loc2) + loc2.Distance(nextLoc1)) * truck1.GetFuelConsumption()
            + (prevLoc2.Distance(loc1) + loc1.Distance(nextLoc2)) * truck2.GetFuelConsumption();

    return currentFuel - newFuel;

}