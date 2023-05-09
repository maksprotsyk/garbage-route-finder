//
// Created by Максим Процик on 24.03.2023.
//

#ifndef ROUTING_PROBLEM_LOCATION_H
#define ROUTING_PROBLEM_LOCATION_H

#include <unordered_map>
#include <functional>

#include "HashableObject.h"

class Location: public HashableObject {
public:
    static bool REAL_DISTANCES;

    static void ClearCache();

    explicit Location();
    Location(double latitude, double longitude);

    void SetLatitude(double latitude);
    void SetLongitude(double longitude);
    [[nodiscard]] double GetLatitude() const;
    [[nodiscard]] double GetLongitude() const;
    double Distance(const Location& location) const;
    bool operator==(const Location& location) const;

    static void AddCachedDistance(const Location& start, const Location& end, double distance);

    friend std::ostream& operator<<(std::ostream& stream, const Location& location);
protected:
    static std::unordered_map<Location, std::unordered_map<Location, double>> CASHED_DISTANCES;
    [[nodiscard]] size_t CalculateHash() const override;
    double CalculateDistance(const Location& location) const;

private:
    double m_latitude = 0;
    double m_longitude = 0;


};

namespace std {
    template <>
    struct hash<Location>
    {
        std::size_t operator()(const Location& loc) const
        {
            return loc.GetHash();
        }
    };
}



#endif //ROUTING_PROBLEM_LOCATION_H
