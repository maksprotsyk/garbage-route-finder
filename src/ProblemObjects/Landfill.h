//
// Created by Максим Процик on 16.04.2023.
//

#ifndef ROUTING_PROBLEM_LANDFILL_H
#define ROUTING_PROBLEM_LANDFILL_H

#include "ProblemObject.h"

class Landfill: public ProblemObject
{
public:
    Landfill(const Location& location, const TimeWindow& timeWindow);
    Landfill() = delete;

    friend std::ostream& operator<<(std::ostream& stream, const Landfill& obj);
};


#endif //ROUTING_PROBLEM_LANDFILL_H
