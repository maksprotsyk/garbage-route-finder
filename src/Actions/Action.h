//
// Created by Максим Процик on 01.05.2023.
//

#ifndef ROUTING_PROBLEM_ACTION_H
#define ROUTING_PROBLEM_ACTION_H

#include "Solution.h"

class Action {
public:
    virtual bool PerformAction(Solution& sol) const = 0;
    virtual double ExpectedDiff(const Solution& sol) const = 0;
    virtual std::set<ObjectId> GetAffectedIds(const Solution& sol) const = 0;
    virtual ~Action() = default;
};


#endif //ROUTING_PROBLEM_ACTION_H
