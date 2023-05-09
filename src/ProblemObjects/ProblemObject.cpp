//
// Created by Максим Процик on 15.04.2023.
//

#include "ProblemObject.h"

ProblemObject::ProblemObject(const Location& location, const TimeWindow& window): m_location(location), m_timeWindow(window)
{
}

const Location &ProblemObject::GetLocation() const
{
    return m_location;
}

const TimeWindow &ProblemObject::GetTimeWindow() const
{
    return m_timeWindow;
}

