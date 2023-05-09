//
// Created by Максим Процик on 01.05.2023.
//

#ifndef ROUTING_PROBLEM_EMPTYACTION_H
#define ROUTING_PROBLEM_EMPTYACTION_H

#include "Heuristic.h"

class EmptyAction: public Action {
public:
    bool PerformAction(Solution& sol) const override;
    std::set<ObjectId> GetAffectedIds(const Solution& sol) const override;
    double ExpectedDiff(const Solution& sol) const override;
};


#endif //ROUTING_PROBLEM_EMPTYACTION_H
