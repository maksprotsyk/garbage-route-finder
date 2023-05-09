//
// Created by Максим Процик on 01.05.2023.
//

#ifndef ROUTING_PROBLEM_REVERSEACTION_H
#define ROUTING_PROBLEM_REVERSEACTION_H

#include "Action.h"

class ReverseAction: public Action
{
public:
    size_t start = 0;
    size_t exportation_index = 0;
    size_t route_index = 0;
    size_t end = 0;

    bool PerformAction(Solution& sol) const override;
    std::set<ObjectId> GetAffectedIds(const Solution& sol) const override;
    double ExpectedDiff(const Solution& sol) const override;
private:
    bool Recalculate(Solution& sol) const;

};


#endif //ROUTING_PROBLEM_REVERSEACTION_H
