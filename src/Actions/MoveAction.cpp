//
// Created by Максим Процик on 01.05.2023.
//

#include "MoveAction.h"

bool MoveAction::PerformAction(Solution& sol) const
{
    if (route_index1 == route_index2 && exportation_index1 == exportation_index2 && container_index1 == container_index2)
    {
        return false;
    }
    if (route_index1 == route_index2 && exportation_index1 == exportation_index2)
    {
        return PerformOneExportation(sol);
    }

    return PerformDifferentExportations(sol);
}

bool MoveAction::PerformOneExportation(Solution &sol) const
{
    if (sol.Routes.size() <= route_index1 || sol.Routes[route_index1].Exportations.size() <= exportation_index1)
    {
        return false;
    }

    if (container_index1 < container_index2)
    {
        Exportation& exportation = sol.Routes[route_index1].Exportations[exportation_index1];
        if (exportation.Containers.size() <= container_index2)
        {
            return false;
        }

        ObjectId container = exportation.Containers[container_index1];
        for (size_t i = container_index1; i < container_index2; i++)
        {
            exportation.Containers[i] = exportation.Containers[i+1];
        }
        exportation.Containers[container_index2] = container;

        if (RecalculateOneExportation(sol))
        {
            sol.InvalidateHash();
            return true;
        }

        for (size_t i = container_index2 - 1; i >= container_index1; i--)
        {
            exportation.Containers[i-1] = exportation.Containers[i];
        }
        exportation.Containers[container_index1] = container;
        RecalculateOneExportation(sol);
        return false;
    }

    Exportation& exportation = sol.Routes[route_index1].Exportations[exportation_index1];
    if (exportation.Containers.size() <= container_index1)
    {
        return false;
    }

    ObjectId container = exportation.Containers[container_index1];
    for (size_t i = container_index1; i >= container_index2 + 1; i--)
    {
        exportation.Containers[i] = exportation.Containers[i-1];
    }
    exportation.Containers[container_index2] = container;
    if (RecalculateOneExportation(sol))
    {
        sol.InvalidateHash();
        return true;
    }

    for (size_t i = container_index2; i < container_index1; i++)
    {
        exportation.Containers[i] = exportation.Containers[i+1];
    }
    exportation.Containers[container_index1] = container;
    RecalculateOneExportation(sol);

    return false;
}

bool MoveAction::PerformDifferentExportations(Solution &sol) const
{
    if (sol.Routes.size() <= route_index1 ||
        sol.Routes[route_index1].Exportations.size() <= exportation_index1 ||
        sol.Routes[route_index1].Exportations[exportation_index1].Containers.size() <= container_index1)
    {
        return false;
    }

    if (sol.Routes.size() <= route_index2 ||
        sol.Routes[route_index2].Exportations.size() <= exportation_index2 ||
        sol.Routes[route_index2].Exportations[exportation_index2].Containers.size() <= container_index2)
    {
        return false;
    }

    Exportation& exportation1 = sol.Routes[route_index1].Exportations[exportation_index1];
    Exportation& exportation2 = sol.Routes[route_index2].Exportations[exportation_index2];
    ObjectId container = exportation1.Containers[container_index1];
    exportation1.Containers.erase(exportation1.Containers.begin() + container_index1);
    exportation2.Containers.insert(exportation2.Containers.begin() + container_index2, container);

    if (RecalculateDifferentExportations(sol))
    {
        sol.InvalidateHash();
        return true;
    }
    exportation2.Containers.erase(exportation2.Containers.begin() + container_index2);
    exportation1.Containers.insert(exportation1.Containers.begin() + container_index1, container);
    RecalculateDifferentExportations(sol);
    return false;
}

bool MoveAction::RecalculateOneExportation(Solution &sol) const
{
    size_t index = std::min(container_index1, container_index2);
    if (!sol.Routes[route_index1].Exportations[exportation_index1].RecalculateInnerValues(index) ||
        !sol.Routes[route_index1].Exportations[exportation_index1].RecalculateFinishValues())
    {
        return false;
    }

    for (size_t i = exportation_index1 + 1; i < sol.Routes[route_index1].Exportations.size(); i++)
    {
        sol.Routes[route_index1].Exportations[i].StartTime = sol.Routes[route_index1].Exportations[i-1].GetFinishTime();
        sol.Routes[route_index1].Exportations[i].StartDistance = sol.Routes[route_index1].Exportations[i-1].GetFinishDistance();
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
    sol.InvalidateHash();
    return true;
}

bool MoveAction::RecalculateDifferentExportations(Solution &sol) const
{
    if (!sol.Routes[route_index1].Exportations[exportation_index1].RecalculateInnerValues(container_index1) ||
        !sol.Routes[route_index1].Exportations[exportation_index1].RecalculateFinishValues())
    {
        return false;
    }

    for (size_t i = exportation_index1 + 1; i < sol.Routes[route_index1].Exportations.size(); i++)
    {
        sol.Routes[route_index1].Exportations[i].StartTime = sol.Routes[route_index1].Exportations[i-1].GetFinishTime();
        sol.Routes[route_index1].Exportations[i].StartDistance = sol.Routes[route_index1].Exportations[i-1].GetFinishDistance();
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
        sol.Routes[route_index2].Exportations[i].StartTime = sol.Routes[route_index2].Exportations[i-1].GetFinishTime();
        sol.Routes[route_index2].Exportations[i].StartDistance = sol.Routes[route_index2].Exportations[i-1].GetFinishDistance();
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

std::set<ObjectId> MoveAction::GetAffectedIds(const Solution& sol) const
{
    return {sol.Routes[route_index1].Exportations[exportation_index1].Containers[container_index1]};
}

double MoveAction::ExpectedDiff(const Solution &sol) const
{
    Problem& problem = Problem::GetProblem();
    Location prevLoc1;
    Location nextLoc1;

    const Exportation& exportation1 = sol.Routes[route_index1].Exportations[exportation_index1];


    size_t realIndex2 = container_index2;
    if (route_index1 == route_index2 && exportation_index1 == exportation_index2 && container_index2 > container_index1)
    {
        realIndex2++;
    }

    ObjectId originalContainer = exportation1.Containers[container_index1];
    Location loc = problem.GetContainer(originalContainer).GetLocation();
    if (container_index1 == 0)
    {
        prevLoc1 = exportation1.StartLocation;
    }
    else
    {
        ObjectId container = exportation1.Containers[container_index1 - 1];
        prevLoc1 = problem.GetContainer(container).GetLocation();
    }

    if (container_index1 + 1 == exportation1.Containers.size())
    {
        nextLoc1 = exportation1.GetFinishLocation();
    }
    else
    {
        ObjectId container = exportation1.Containers[container_index1 + 1];
        nextLoc1 = problem.GetContainer(container).GetLocation();
    }
    const Exportation& exportation2 = sol.Routes[route_index2].Exportations[exportation_index2];

    Location prevLoc2;
    Location nextLoc2;
    if (realIndex2 == 0)
    {
        prevLoc2 = exportation2.StartLocation;
    }
    else
    {
        ObjectId container = exportation2.Containers[realIndex2 - 1];
        prevLoc2 = problem.GetContainer(container).GetLocation();
    }

    if (realIndex2 == exportation2.Containers.size())
    {
        nextLoc2 = exportation2.GetFinishLocation();
    }
    else
    {
        ObjectId container = exportation2.Containers[realIndex2];
        nextLoc2 = problem.GetContainer(container).GetLocation();
    }

    double originalDistance = prevLoc1.Distance(loc) + loc.Distance(nextLoc1) + prevLoc2.Distance(nextLoc2);
    double newDistance = prevLoc1.Distance(nextLoc1) + prevLoc2.Distance(loc) + loc.Distance(nextLoc2);
    return  originalDistance - newDistance;
}
