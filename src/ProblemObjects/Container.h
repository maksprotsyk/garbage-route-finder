//
// Created by Максим Процик on 16.04.2023.
//

#ifndef ROUTING_PROBLEM_CONTAINER_H
#define ROUTING_PROBLEM_CONTAINER_H

#include "ProblemObject.h"

class Container: public ProblemObject
{
public:
    struct TypeData {
        std::string typeName;
        float processingTime;
        std::string garbageType;
        float capacity;

    };

    Container(const Location& location, const TimeWindow& window);
    Container() = delete;

    double GetCapacity() const;
    double GetProcessingTime() const;
    std::string GetGarbageType() const;
    std::string GetType() const;

    void SetTypeData(const TypeData& data);

    friend std::ostream& operator<<(std::ostream& stream, const Container& obj);
protected:
    std::string m_type = "Unknown";
    std::string m_garbageType = "Unknown";
    double m_capacity = 0.0f;
    // time needed to process container
    double m_processingTime = 0;

};


#endif //ROUTING_PROBLEM_CONTAINER_H
