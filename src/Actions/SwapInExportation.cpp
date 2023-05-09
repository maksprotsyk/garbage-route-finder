//
// Created by Максим Процик on 01.05.2023.
//

#include "SwapInExportation.h"


std::vector<std::pair<std::shared_ptr<Action>, std::shared_ptr<Action>>> SwapInExportation::GetNeighboursActions(
        const Solution& solution, const std::unordered_set<ObjectId>& forbiddenIds
        )
{
    std::vector<std::pair<std::shared_ptr<Action>, std::shared_ptr<Action>>> actions;
    for (size_t x = 0; x < solution.Routes.size(); x++) {
        for (size_t y = 0; y < solution.Routes[x].Exportations.size(); y++) {
            const Exportation& exportation = solution.Routes[x].Exportations[y];
            for (size_t i = 0; i < exportation.Containers.size(); i++) {
                if (forbiddenIds.find(exportation.Containers[i]) != forbiddenIds.end())
                {
                    continue;
                }

                for (size_t j = i + 1; j < exportation.Containers.size(); j++) {
                    if (forbiddenIds.find(exportation.Containers[j]) != forbiddenIds.end())
                    {
                        continue;
                    }

                    auto action = std::make_shared<InterChangeAction>();
                    action->route_index1 = x;
                    action->route_index2 = x;
                    action->exportation_index1 = y;
                    action->exportation_index2 = y;
                    action->container_index1 = i;
                    action->container_index2 = j;
                    auto reverse_action = std::make_shared<InterChangeAction>();
                    reverse_action->route_index1 = x;
                    reverse_action->route_index2 = x;
                    reverse_action->exportation_index1 = y;
                    reverse_action->exportation_index2 = y;
                    reverse_action->container_index1 = i;
                    reverse_action->container_index2 = j;

                    actions.emplace_back(action, reverse_action);
                }
            }
        }
    }
    return actions;
}
