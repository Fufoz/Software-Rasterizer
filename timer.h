#ifndef TIMER_H
#define TIMER_H

#include <chrono>

struct Timer {

    void start()
    {
        m_StartTime = std::chrono::system_clock::now();
    }

    double stopMs()
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - m_StartTime).count();
    }

    std::chrono::time_point<std::chrono::system_clock> m_StartTime;
};



#endif