//
// Created by Максим Процик on 01.05.2023.
//

#include "EmptyAction.h"

bool EmptyAction::PerformAction(Solution &sol) const
{
    return true;
}

std::set<ObjectId> EmptyAction::GetAffectedIds(const Solution& sol) const
{
    return std::set<ObjectId>();
}

double EmptyAction::ExpectedDiff(const Solution& sol) const
{
    return 0.0;
}