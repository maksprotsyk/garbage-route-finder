//
// Created by Максим Процик on 02.04.2023.
//

#include "GarbageTruck.h"

GarbageTruck::GarbageTruck(const Location& location, const TimeWindow& window): ProblemObject(location, window) {}

double GarbageTruck::GetSpeed() const
{
    return m_speed;
}

double GarbageTruck::GetFuelConsumption() const
{
    return m_fuelConsumption;
}

double GarbageTruck::GetUnloadingTime() const
{
    return m_unloadingTime;
}

double GarbageTruck::GetFuelCapacity() const
{
    return m_fuelCapacity;
}

const std::unordered_map<std::string, double>& GarbageTruck::GetCapacities() const
{
    return m_capacities;
}


void GarbageTruck::SetTypeData(const TypeData& data)
{
    m_type = data.typeName;
    m_capacities = data.capacities;
    m_fuelCapacity = data.fuelCapacity;
    m_fuelConsumption = data.fuelConsumption;
    m_speed = data.speed;
    m_unloadingTime = data.unloadingTime;
}

std::string GarbageTruck::GetType() const
{
    return m_type;
}

std::ostream &operator<<(std::ostream &stream, const GarbageTruck &obj)
{
    std::string spaces(stream.width(), ' ');
    stream
            << spaces
            << "GarbageTruck("
            << obj.m_type
            << ", "
            << obj.GetLocation()
            << ", "
            << obj.GetTimeWindow()
            << ")";
    return stream;
}
