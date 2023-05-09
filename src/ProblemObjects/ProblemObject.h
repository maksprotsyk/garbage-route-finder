//
// Created by Максим Процик on 15.04.2023.
//

#ifndef ROUTING_PROBLEM_PROBLEMOBJECT_H
#define ROUTING_PROBLEM_PROBLEMOBJECT_H

#include "Location.h"
#include "TimeWindow.h"
#include "HashableObject.h"

class ProblemObject
{
public:
    ProblemObject(const Location& location, const TimeWindow& window);
    ProblemObject() = delete;
    const Location& GetLocation() const;
    const TimeWindow& GetTimeWindow() const;

protected:
    Location m_location;
    TimeWindow m_timeWindow;

};

#endif //ROUTING_PROBLEM_PROBLEMOBJECT_H
