//
// Created by Максим Процик on 16.04.2023.
//

#ifndef ROUTING_PROBLEM_EXPORTATION_H
#define ROUTING_PROBLEM_EXPORTATION_H

#include "Problem.h"


class Exportation
{
public:

    std::vector<ObjectId> Containers;
    ObjectId Truck = -1;
    ObjectId CurrentLandfill = -1;
    double StartTime = 0;
    double StartDistance = 0;
    Location StartLocation;

    Location GetFinishLocation() const;
    double GetFinishTime() const;
    double GetFinishDistance() const;

    // calculates finish time and finish distance
    // returns false in case of error
    // !! assumes that all vectors are already recalculated
    bool RecalculateFinishValues();
    bool RecalculateInnerValues(size_t start);


    bool RecalculateVolumes(size_t start);
    bool RecalculateDistances(size_t start);
    bool RecalculateFinishTimes(size_t start);

    friend std::ostream& operator<<(std::ostream& stream, const Exportation& obj);


protected:
    // volumes of different garbage types in the truck at every container location visited during the exportation
    std::vector<std::unordered_map<std::string, double>> m_volumes;

    // time of processing finish for every container
    std::vector<double> m_finishTimes;

    // total distance traveled by the truck before every container location visited during the exportation
    std::vector<double> m_distances;
    // time after unloading at the landfill
    double m_finishTime = 0;

    // total distance traveled before we got to the landfill
    double m_finishDistance = 0;
};

#endif //ROUTING_PROBLEM_EXPORTATION_H
