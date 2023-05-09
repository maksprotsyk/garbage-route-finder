//
// Created by Максим Процик on 01.05.2023.
//

#ifndef ROUTING_PROBLEM_INTERCHANGEACTION_H
#define ROUTING_PROBLEM_INTERCHANGEACTION_H

#include "Action.h"

class InterChangeAction: public Action {
public:
    size_t container_index1 = 0;
    size_t exportation_index1 = 0;
    size_t route_index1 = 0;
    size_t container_index2 = 0;
    size_t exportation_index2 = 0;
    size_t route_index2 = 0;

    bool PerformAction(Solution& sol) const override;
    std::set<ObjectId> GetAffectedIds(const Solution& sol) const override;
    double ExpectedDiff(const Solution& sol) const override;
private:
    bool Recalculate(Solution& sol) const;
};


#endif //ROUTING_PROBLEM_INTERCHANGEACTION_H
