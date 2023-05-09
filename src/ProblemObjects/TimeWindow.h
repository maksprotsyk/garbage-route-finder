//
// Created by Максим Процик on 24.03.2023.
//

#ifndef ROUTING_PROBLEM_TIMEWINDOW_H
#define ROUTING_PROBLEM_TIMEWINDOW_H

#include <ostream>

class TimeWindow {
public:
    explicit TimeWindow();
    TimeWindow(double start, double end);

    void SetStart(double start);
    void SetEnd(double end);
    double GetStart() const;
    double GetEnd() const;

    friend std::ostream& operator<<(std::ostream& stream, const TimeWindow& window);

private:
    double m_start;
    double m_end;
};


#endif //ROUTING_PROBLEM_TIMEWINDOW_H
