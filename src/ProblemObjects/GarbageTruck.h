//
// Created by Максим Процик on 02.04.2023.
//

#ifndef ROUTING_PROBLEM_GARBAGETRUCK_H
#define ROUTING_PROBLEM_GARBAGETRUCK_H

#include "ProblemObject.h"

class GarbageTruck: public ProblemObject
{
public:
    struct TypeData {
        std::string typeName;
        double speed;
        double fuelConsumption;
        double fuelCapacity;
        double unloadingTime;
        std::unordered_map<std::string, double> capacities;
    };

    GarbageTruck(const Location& location, const TimeWindow& window);
    GarbageTruck() = delete;

    double GetSpeed() const;
    double GetFuelConsumption() const;
    double GetFuelCapacity() const;
    const std::unordered_map<std::string, double>& GetCapacities() const;
    double GetUnloadingTime() const;
    std::string GetType() const;

    void SetTypeData(const TypeData& data);

    friend std::ostream& operator<<(std::ostream& stream, const GarbageTruck& obj);

protected:
    std::string m_type = "Unknown";

    double m_speed = 0;

    // "fuel consumption" * "distance" = "fuel used"
    double m_fuelConsumption = 0;

    double m_fuelCapacity = 0;

    // capacities of garbage types that can be stored in the truck
    std::unordered_map<std::string, double> m_capacities;

    // time needed to unload the truck on the landfill
    double m_unloadingTime = 0;
};


#endif //ROUTING_PROBLEM_GARBAGETRUCK_H
