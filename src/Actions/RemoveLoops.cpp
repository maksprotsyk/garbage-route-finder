//
// Created by Максим Процик on 01.05.2023.
//

#include "RemoveLoops.h"


static int get_relative_position(const Location& A, const Location& B, const Location& C) {
    double val1 = (C.GetLongitude() - A.GetLongitude()) * (B.GetLatitude() - A.GetLatitude());
    double val2 = (B.GetLongitude() - A.GetLongitude()) * (C.GetLatitude() - A.GetLatitude());
    // C is higher than it should be
    if (val1 > val2) {
        return 1;
    } else if (val1 == val2) {
        return 0;
    } else {
        return -1;
    }
}


static bool check_intersection(const Location& A, const Location& B, const Location& C, const Location& D) {
    int pos_a = get_relative_position(A, C, D);
    int pos_b = get_relative_position(B, C, D);
    int pos_c = get_relative_position(C, A, B);
    int pos_d = get_relative_position(D, A, B);
    if (pos_a != 0 && pos_b != 0 && pos_c != 0) {
        return pos_a != pos_b && pos_c != pos_d;
    }

    if (pos_a == 0) {
        if (A.GetLatitude() >= C.GetLatitude() && A.GetLatitude() <= D.GetLatitude()) {
            return true;
        } else if (A.GetLatitude() <= C.GetLatitude() && A.GetLatitude() >= D.GetLatitude()) {
            return true;
        }
    }
    if (pos_b == 0) {
        if (B.GetLatitude() >= C.GetLatitude() && B.GetLatitude() <= D.GetLatitude()) {
            return true;
        } else if (B.GetLatitude() <= C.GetLatitude() && B.GetLatitude() >= D.GetLatitude()) {
            return true;
        }

    }

    if (pos_c == 0) {
        if (C.GetLatitude() >= A.GetLatitude() && C.GetLatitude() <= B.GetLatitude()) {
            return true;
        } else if (C.GetLatitude() <= A.GetLatitude() && C.GetLatitude() >= B.GetLatitude()) {
            return true;
        }

    }

    if (pos_d == 0) {
        if (D.GetLatitude() >= A.GetLatitude() && D.GetLatitude() <= B.GetLatitude()) {
            return true;
        } else if (D.GetLatitude() <= A.GetLatitude() && D.GetLatitude() >= B.GetLatitude()) {
            return true;
        }

    }

    return false;
}


std::vector<std::pair<std::shared_ptr<Action>, std::shared_ptr<Action>>> RemoveLoops::GetNeighboursActions(
        const Solution& solution, const std::unordered_set<ObjectId>& forbiddenIds
        )
{
    Problem& problem = Problem::GetProblem();
    std::vector<std::pair<std::shared_ptr<Action>, std::shared_ptr<Action>>> actions;
    for (size_t x = 0; x < solution.Routes.size(); x++) {
        for (size_t y = 0; y < solution.Routes[x].Exportations.size(); y++) {
            const Exportation& exportation = solution.Routes[x].Exportations[y];
            for (size_t i = 0; i + 1 < exportation.Containers.size(); i++) {
                for (size_t j = i + 2; j + 1 < exportation.Containers.size(); j++) {
                    const Location& A = problem.GetContainer(exportation.Containers[i]).GetLocation();
                    const Location& B = problem.GetContainer(exportation.Containers[i+1]).GetLocation();
                    const Location& C = problem.GetContainer(exportation.Containers[j]).GetLocation();
                    const Location& D = problem.GetContainer(exportation.Containers[j+1]).GetLocation();
                    if (check_intersection(A, B, C, D)) {
                        auto action = std::make_shared<ReverseAction>();
                        action->route_index = x;
                        action->exportation_index = y;
                        action->start = i+1;
                        action->end = j;

                        bool isForbidden = false;
                        for (size_t index = action->start; index <= action->end; index++)
                        {
                            if (forbiddenIds.find(exportation.Containers[index]) != forbiddenIds.end())
                            {
                                isForbidden = true;
                                break;
                            }
                        }
                        if (isForbidden)
                        {
                            continue;
                        }

                        auto reverse_action = std::make_shared<ReverseAction>();
                        reverse_action->route_index = x;
                        reverse_action->exportation_index = y;
                        reverse_action->start = i+1;
                        reverse_action->end = j;

                        actions.emplace_back(action, reverse_action);
                    }

                }
            }
        }
    }

    return actions;
}
