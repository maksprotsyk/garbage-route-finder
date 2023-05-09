//
// Created by Максим Процик on 01.05.2023.
//

#ifndef ROUTING_PROBLEM_CLUSTERSOLUTION_H
#define ROUTING_PROBLEM_CLUSTERSOLUTION_H

#include "BaseSolution.h"

class ClusterSolution: public BaseSolution
{
public:
    double clusterCoef = 1.0;
    Solution CreateSolution() const override;
private:
    static std::vector<int> HierarchicalAgglomerativeClustering(double coef);
};


#endif //ROUTING_PROBLEM_CLUSTERSOLUTION_H
