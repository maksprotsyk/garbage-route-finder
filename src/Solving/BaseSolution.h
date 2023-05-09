//
// Created by Максим Процик on 30.04.2023.
//

#ifndef ROUTING_PROBLEM_BASESOLUTION_H
#define ROUTING_PROBLEM_BASESOLUTION_H

#include "Solution.h"

class BaseSolution
{
public:
    virtual Solution CreateSolution() const = 0;
};


#endif //ROUTING_PROBLEM_BASESOLUTION_H
