//
// Created by Максим Процик on 30.04.2023.
//

#ifndef ROUTING_PROBLEM_GREEDYSOLUTION_H
#define ROUTING_PROBLEM_GREEDYSOLUTION_H

#include "BaseSolution.h"

class GreedySolution: public BaseSolution
{
public:
    Solution CreateSolution() const override;
};


#endif //ROUTING_PROBLEM_GREEDYSOLUTION_H
