//
// Created by Максим Процик on 16.04.2023.
//

#include "Container.h"

Container::Container(const Location& location, const TimeWindow& window): ProblemObject(location, window)
{

}

double Container::GetCapacity() const
{
    return m_capacity;
}

double Container::GetProcessingTime() const
{
    return m_processingTime;
}

std::string Container::GetGarbageType() const
{
    return m_garbageType;
}

void Container::SetTypeData(const Container::TypeData &data)
{
    m_type = data.typeName;
    m_garbageType = data.garbageType;
    m_capacity = data.capacity;
    m_processingTime = data.processingTime;
}

std::string Container::GetType() const
{
    return m_type;
}

std::ostream &operator<<(std::ostream &stream, const Container &obj)
{
    std::string spaces(stream.width(), ' ');
    stream
            << spaces
            << "Container("
            << obj.m_type
            << ", "
            << obj.GetLocation()
            << ", "
            << obj.GetTimeWindow()
            << ")";
    return stream;
}