//
// Created by Максим Процик on 17.04.2023.
//

#include "Route.h"

double Route::FinishDistance() const
{
    return m_finishDistance;
}

double Route::FinishTime() const
{
    return m_finishTime;
}

double Route::FinishFuel() const
{
    return m_finishFuel;
}

bool Route::CalculateFinishValues()
{
    Problem& problem = Problem::GetProblem();
    const GarbageTruck& truck = problem.GetTruck(Truck);
    if (Exportations.empty()) {
        m_finishDistance = 0;
        m_finishTime = truck.GetTimeWindow().GetStart();
        m_finishFuel = 0;
        return true;
    }

    const Landfill& landfill =  problem.GetLandfill(Exportations.back().CurrentLandfill);
    double backDistance = landfill.GetLocation().Distance(truck.GetLocation());
    m_finishDistance = backDistance + Exportations.back().GetFinishDistance();
    m_finishFuel = m_finishDistance * truck.GetFuelConsumption();
    if (m_finishFuel > truck.GetFuelCapacity())
    {
        return false;
    }
    m_finishTime = backDistance / truck.GetSpeed() + Exportations.back().GetFinishTime();
    if (m_finishTime > truck.GetTimeWindow().GetEnd())
    {
        return false;
    }
    return true;

}


std::ostream& operator<<(std::ostream& stream, const Route& route)
{
    size_t width = stream.width();
    std::string line(width, ' ');
    Problem& problem = Problem::GetProblem();
    const GarbageTruck& truck = problem.GetTruck(route.Truck);

    stream << line;
    stream
            << "Route(\n"
            << line
            << "--truck=";

    if (route.Truck < 0)
    {
        stream << "null";
    }
    else
    {
        stream << truck;
    }

    stream << ",\n" << line << "--Exportations: {\n";

    for (const auto& exportation: route.Exportations)
    {
        stream<< std::setw(width + 4) << exportation << ",\n";
    }
    stream
            << line
            << "--}\n"
            << line
            << ")";

    return stream;

}
