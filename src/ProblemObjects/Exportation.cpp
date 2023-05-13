//
// Created by Максим Процик on 16.04.2023.
//

#include "Exportation.h"


Location Exportation::GetFinishLocation() const
{
    return Problem::GetProblem().GetLandfill(CurrentLandfill).GetLocation();
}

double Exportation::GetFinishTime() const
{
    return m_finishTime;
}

double Exportation::GetFinishDistance() const
{
    return m_finishDistance;
}

bool Exportation::RecalculateVolumes(size_t start)
{

    if (Containers.size() <= start)
    {
        return true;
    }

    if (m_volumes.size() > Containers.size())
    {
        m_volumes.erase(m_volumes.begin() + Containers.size(), m_volumes.end());
    }
    else if (m_volumes.size() < Containers.size())
    {
        m_volumes.reserve(Containers.size() - m_volumes.size());
        for (size_t i = m_volumes.size(); i < Containers.size(); i++)
        {
            m_volumes.emplace_back();
        }
    }

    m_volumes[start].clear();

    Problem& problem = Problem::GetProblem();

    const GarbageTruck& truck = problem.GetTruck(Truck);

    std::unordered_map<std::string, double> truckCapacities = truck.GetCapacities();

    if (start == 0)
    {
        const Container& container = problem.GetContainer(Containers[0]);
        m_volumes[0][container.GetGarbageType()] = container.GetCapacity();
        if (truckCapacities[container.GetGarbageType()] < container.GetCapacity())
        {
            return false;
        }
        start++;
    }

    for (size_t i = start; i < m_volumes.size(); i++)
    {
        m_volumes[i].clear();
        for (auto& [key, value]: m_volumes[i-1])
        {
            m_volumes[i][key] = value;
        }
        const Container& container = problem.GetContainer(Containers[i]);
        m_volumes[i][container.GetGarbageType()] += container.GetCapacity();

        if (truckCapacities[container.GetGarbageType()] < m_volumes[i][container.GetGarbageType()])
        {
            return false;
        }

    }

    return true;

}

bool Exportation::RecalculateDistances(size_t start)
{

    if (Containers.size() <= start)
    {
        return true;
    }

    if (m_distances.size() > Containers.size())
    {
        m_distances.erase(m_distances.begin() + Containers.size(), m_distances.end());
    }
    else if (m_distances.size() < Containers.size())
    {
        m_distances.reserve(Containers.size() - m_distances.size());
        for (size_t i = m_distances.size(); i < Containers.size(); i++)
        {
            m_distances.emplace_back(0);
        }
    }

    Problem& problem = Problem::GetProblem();
    const GarbageTruck& truck = problem.GetTruck(Truck);

    if (start == 0)
    {
        const Container& container = problem.GetContainer(Containers[0]);
        m_distances[0] = StartLocation.Distance(container.GetLocation()) + StartDistance;
        if (m_distances[0] * truck.GetFuelConsumption() > truck.GetFuelCapacity())
        {
            return false;
        }
        start++;
    }

    for (size_t i = start; i < m_volumes.size(); i++)
    {
        const Container& prevContainer = problem.GetContainer(Containers[i - 1]);
        const Container& container = problem.GetContainer(Containers[i]);
        m_distances[i] = m_distances[i - 1] + prevContainer.GetLocation().Distance(container.GetLocation());
        if (m_distances[i] * truck.GetFuelConsumption() > truck.GetFuelCapacity())
        {
            return false;
        }

    }
    return true;
}

bool Exportation::RecalculateFinishTimes(size_t start)
{

    if (Containers.size() <= start)
    {
        return true;
    }

    if (m_finishTimes.size() > Containers.size())
    {
        m_finishTimes.erase(m_finishTimes.begin() + Containers.size(), m_finishTimes.end());
    }
    else if (m_finishTimes.size() < Containers.size())
    {
        m_finishTimes.reserve(Containers.size() - m_finishTimes.size());
        for (size_t i = m_finishTimes.size(); i < Containers.size(); i++)
        {
            m_finishTimes.emplace_back();
        }

    }

    Problem &problem = Problem::GetProblem();
    const GarbageTruck &truck = problem.GetTruck(Truck);

    float speed = truck.GetSpeed();
    for (size_t i = start; i < m_finishTimes.size(); i++)
    {
        const Container &container = problem.GetContainer(Containers[i]);
        double startTime;
        if (i != 0)
        {
            startTime = (m_distances[i] - m_distances[i-1]) / speed + m_finishTimes[i-1];
        }
        else
        {
            startTime = (m_distances[i] - StartDistance) / speed + StartTime;
        }
        m_finishTimes[i] = std::max(startTime, container.GetTimeWindow().GetStart()) + container.GetProcessingTime();
        if (m_finishTimes[i] > container.GetTimeWindow().GetEnd() || m_finishTimes[i] > truck.GetTimeWindow().GetEnd())
        {
            return false;
        }

    }
    return true;
}

bool Exportation::RecalculateFinishValues()
{
    Problem &problem = Problem::GetProblem();

    double totalDistance;
    double time;
    double distanceToLandfill;

    const Landfill& landfill = problem.GetLandfill(CurrentLandfill);
    if (Containers.empty())
    {
        distanceToLandfill = StartLocation.Distance(landfill.GetLocation());
        totalDistance = distanceToLandfill;
        time = StartTime;
    }
    else
    {
        const Container& container = problem.GetContainer(Containers.back());
        distanceToLandfill = container.GetLocation().Distance(landfill.GetLocation());
        totalDistance = distanceToLandfill + m_distances.back();
        time = m_finishTimes.back();
    }

    m_finishDistance = totalDistance;
    const GarbageTruck& truck = problem.GetTruck(Truck);
    if (m_finishDistance * truck.GetFuelConsumption() >= truck.GetFuelCapacity())
    {
        return false;
    }
    m_finishTime = std::max(time + distanceToLandfill / truck.GetSpeed(), landfill.GetTimeWindow().GetStart()) + truck.GetUnloadingTime();
    return m_finishTime <= landfill.GetTimeWindow().GetEnd() && m_finishTime <= truck.GetTimeWindow().GetEnd();
}

bool Exportation::RecalculateInnerValues(size_t start)
{
    bool res = RecalculateVolumes(start);
    if (!res) {
        return false;
    }
    res = RecalculateDistances(start);
    if (!res) {
        return false;
    }
    res = RecalculateFinishTimes(start);
    return res;
}

std::ostream& operator<<(std::ostream& stream, const Exportation& exportation)
{
    std::string line(stream.width(), ' ');
    stream << line;

    Problem &problem = Problem::GetProblem();

    stream << "Exportation(\n";

    stream << line << "  truck=";

    if (exportation.Truck < 0)
    {
        stream << "null";
    }
    else
    {
        stream << problem.GetTruck(exportation.Truck);
    }

    stream << ",\n" << line << "  landfill=";
    if (exportation.CurrentLandfill < 0)
    {
        stream << "null";
    }
    else
    {
        stream << problem.GetLandfill(exportation.CurrentLandfill);
    }

    stream
            << ",\n\n"
            << line
            << "  start_time="
            << exportation.StartTime
            << ",\n"
            << line
            << "  start_distance="
            << exportation.StartDistance
            << ",\n"
            << line
            << "  start_location="
            << exportation.StartLocation
            << ",\n\n"
            << line
            << "  finish_time="
            << exportation.m_finishTime
            << ",\n"
            << line
            << "  finish_distance="
            << exportation.m_finishDistance
            << ",\n\n" << line <<"  Containers: {\n";

    for (ObjectId containerId: exportation.Containers) {
        stream << line << "    " << problem.GetContainer(containerId) << ",\n";
    }
    stream
            << line
            << "  }\n"
            << line
            << ")";

    return stream;

}
