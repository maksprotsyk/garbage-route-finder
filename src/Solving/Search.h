//
// Created by Максим Процик on 01.05.2023.
//

#ifndef ROUTING_PROBLEM_SEARCH_H
#define ROUTING_PROBLEM_SEARCH_H

#include "Heuristic.h"

class Search {
public:
    std::shared_ptr<Heuristic> heuristic = nullptr;
    virtual Solution search(const Solution& sol) = 0;
protected:
};


#endif //ROUTING_PROBLEM_SEARCH_H
