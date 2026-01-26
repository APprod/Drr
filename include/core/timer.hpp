#pragma once
#include <chrono>
#include <unordered_map>
#include <string>

namespace MyTime
{
    using Clock=std::chrono::steady_clock;
    using Micro=std::chrono::microseconds;
} 


class Timer
{    
public:
    Timer();
    void init(float millis);
    
    bool check();
    float get();
    ~Timer();

private:
    MyTime::Clock::time_point m_start;
    MyTime::Micro m_dur;
};


class TimersSystem
{
public:
    bool createTimer(std::string name, float millis);
    bool removeTimer(std::string name);
    Timer& getTimer(std::string name, bool &ok);
private:
    std::unordered_map<std::string, Timer> m_map;
};
