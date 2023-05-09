//
// Created by Максим Процик on 01.05.2023.
//

#include "SwapAny.h"



std::vector<std::pair<std::shared_ptr<Action>, std::shared_ptr<Action>>> SwapAny::GetNeighboursActions(
        const Solution& solution, const std::unordered_set<ObjectId>& forbiddenIds)
{
    std::vector<std::pair<std::shared_ptr<Action>, std::shared_ptr<Action>>> actions;
    std::vector<ObjectId> allowedIds;

    for (ObjectId id = 0; id < Problem::GetProblem().GetContainersNum(); id++)
    {
        if (forbiddenIds.find(id) != forbiddenIds.end())
        {
            continue;
        }
        allowedIds.push_back(id);
    }
    std::unordered_set<ObjectId> containers;
    size_t requiredSize = std::min(to_generate, allowedIds.size());
    for (size_t i = 0; i < requiredSize; i++)
    {
        std::uniform_int_distribution<> containersDistr(0, (int)(allowedIds.size() - 1));
        int index = containersDistr(rd);
        ObjectId id = allowedIds[index];
        containers.insert(id);
        allowedIds.erase(allowedIds.begin() + index);
    }

    std::vector<ContainerLocation> allLocations;
    allLocations.reserve(Problem::GetProblem().GetContainersNum());
    std::vector<ContainerLocation> requiredLocations;

    for (size_t r = 0; r < solution.Routes.size(); r++)
    {
        for (size_t e = 0; e < solution.Routes[r].Exportations.size(); e++)
        {
            for (size_t c = 0; c < solution.Routes[r].Exportations[e].Containers.size(); c++)
            {
                ObjectId id = solution.Routes[r].Exportations[e].Containers[c];
                ContainerLocation location;
                location.routeIndex = r;
                location.expIndex = e;
                location.index = c;
                location.id = id;
                if (forbiddenIds.find(id) != forbiddenIds.end())
                {
                    continue;
                }
                allLocations.push_back(location);
                if (containers.find(id) != containers.end())
                {
                    requiredLocations.push_back(location);
                }
            }
        }
    }

    for (const auto& requiredLoc: requiredLocations)
    {
        for (const auto& loc: allLocations)
        {
            if (containers.find(loc.id) != containers.end() && loc.id >= requiredLoc.id)
            {
                continue;
            }
            auto action = std::make_shared<InterChangeAction>();
            action->route_index1 = loc.routeIndex;
            action->route_index2 = requiredLoc.routeIndex;
            action->exportation_index1 = loc.expIndex;
            action->exportation_index2 = requiredLoc.expIndex;
            action->container_index1 = loc.index;
            action->container_index2 = requiredLoc.index;

            auto reverse_action = std::make_shared<InterChangeAction>();
            reverse_action->route_index1 = action->route_index1;
            reverse_action->route_index2 = action->route_index2;
            reverse_action->exportation_index1 = action->exportation_index1;
            reverse_action->exportation_index2 = action->exportation_index2;
            reverse_action->container_index1 = action->container_index1;
            reverse_action->container_index2 = action->container_index2;

            actions.emplace_back(action, reverse_action);
        }
    }

    return actions;
}
