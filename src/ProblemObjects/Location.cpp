//
// Created by Максим Процик on 24.03.2023.
//

#include "Location.h"

bool Location::REAL_DISTANCES = false;

std::unordered_map<Location, std::unordered_map<Location, double>> Location::CASHED_DISTANCES{};

void Location::ClearCache()
{
    CASHED_DISTANCES.clear();
}

Location::Location(): Location(0.0, 0.0)
{

}

Location::Location(double latitude, double longitude): m_latitude(latitude), m_longitude(longitude)
{

}

void Location::SetLatitude(double latitude)
{
    m_latitude = latitude;
    InvalidateHash();
}

void Location::SetLongitude(double longitude)
{
    m_longitude = longitude;
    InvalidateHash();
}

double Location::GetLatitude() const
{
    return m_latitude;
}

double Location::GetLongitude() const
{
    return m_longitude;
}

double Location::Distance(const Location &location) const
{
    auto itr = CASHED_DISTANCES.find(*this);
    if (itr == CASHED_DISTANCES.end())
    {
        if (REAL_DISTANCES)
        {
            throw std::runtime_error("Not cached real distance");
        }

        double distance = CalculateDistance(location);
        CASHED_DISTANCES[*this][location] = distance;
        return distance;

    }

    auto itr_inner = itr->second.find(location);
    if (itr_inner == itr->second.end())
    {
        if (REAL_DISTANCES)
        {
            throw std::runtime_error("Not cached real distance");
        }

        double distance = CalculateDistance(location);
        CASHED_DISTANCES[*this][location] = distance;
        return distance;
    }

    return itr_inner->second;
}


bool Location::operator==(const Location& location) const
{
    double epsilon = 0.0000000001;
    return std::abs(location.m_latitude - m_latitude) < epsilon && std::abs(location.m_longitude - m_longitude) < epsilon;
}


size_t Location::CalculateHash() const
{
    size_t hash = 0;
    boost::hash_combine(hash, m_latitude);
    boost::hash_combine(hash, m_longitude);
    return hash;
}

double Location::CalculateDistance(const Location &location) const
{
    return std::sqrt(
            std::pow(location.m_latitude - m_latitude, 2)
            + std::pow(location.m_longitude - m_longitude, 2));

}

std::ostream &operator<<(std::ostream &stream, const Location &location)
{
    std::string spaces(stream.width(), ' ');
    stream
            << spaces
            << "Location(lat="
            << location.m_latitude
            << ", lon="
            << location.m_longitude
            << ")";

    return stream;
}

void Location::AddCachedDistance(const Location &start, const Location &end, double distance)
{
    CASHED_DISTANCES[start][end] = distance;
}

