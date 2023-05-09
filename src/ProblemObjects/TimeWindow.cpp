//
// Created by Максим Процик on 24.03.2023.
//

#include "TimeWindow.h"

TimeWindow::TimeWindow(): TimeWindow(0.0f, 10000000.0f)
{

}

TimeWindow::TimeWindow(double start, double end): m_start(start), m_end(end)
{

}

void TimeWindow::SetStart(double start)
{
    m_start = start;
}

void TimeWindow::SetEnd(double end)
{
    m_end = end;
}

double TimeWindow::GetStart() const
{
    return m_start;
}

double TimeWindow::GetEnd() const
{
    return m_end;
}

std::ostream& operator<<(std::ostream& stream, const TimeWindow& window)
{
    std::string spaces(stream.width(), ' ');
    stream
            << spaces
            << "TimeWindow(s="
            << window.m_start
            << ", e="
            << window.m_end
            << ")";

    return stream;
}