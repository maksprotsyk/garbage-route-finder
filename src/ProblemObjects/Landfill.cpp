//
// Created by Максим Процик on 16.04.2023.
//

#include "Landfill.h"

Landfill::Landfill(const Location &location, const TimeWindow &timeWindow) : ProblemObject(location, timeWindow)
{

}

std::ostream &operator<<(std::ostream &stream, const Landfill &obj)
{
    std::string spaces(stream.width(), ' ');
    stream
            << spaces
            << "Landfill("
            << obj.GetLocation()
            << ", "
            << obj.GetTimeWindow()
            << ")";
    return stream;
}
