//
// Created by Максим Процик on 17.04.2023.
//

#ifndef ROUTING_PROBLEM_ROUTE_H
#define ROUTING_PROBLEM_ROUTE_H

#include "Exportation.h"


class Route {
public:

    ObjectId Truck = -1;
    std::vector<Exportation> Exportations;

    // calculates total distance traveled during the route
    // assumes that all exportations have their info vectors calculated
    double FinishDistance() const;

    // calculates finish time of the route
    double FinishTime() const;

    // calculates total fuel spent during this route
    double FinishFuel() const;


    bool CalculateFinishValues();

    friend std::ostream& operator<<(std::ostream& stream, const Route& obj);
private:
    double m_finishTime;
    double m_finishFuel;
    double m_finishDistance;



};

#endif //ROUTING_PROBLEM_ROUTE_H
