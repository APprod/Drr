#pragma once

#include <chrono>
#include <functional>
#include <numeric>
#include <string>
#include <unordered_map>
#include <vector>

class PerformanceLog;

// Monitors function execution time. Logs if it goes over the treshold 
class Tester
{
    std::chrono::steady_clock::time_point m_start;
    std::string m_name;
    float m_thresholdMs;
    bool m_active;
public:

    Tester(std::string name, float thresholdMs, bool active);
    ~Tester();
};

/* preformance logging*/

//helper class
struct PerfStat{
    std::vector<float> deltas;
    float average() const {return deltas.size() ? (std::accumulate(deltas.begin(), deltas.end(), 0.0f) / deltas.size()) : 0 ;}
    float peak() const {return deltas.size() ? (*std::max_element(deltas.begin(), deltas.end())) : 0;}
};
// RAII object, couns time between creation and descruction; put it at the start of tested scope
class PerfTester
{
    friend class PerformanceLog; //we cannot create PerfTest without existing PerformanceLog
    std::function<void(std::string, float)> m_callback;
    std::chrono::steady_clock::time_point m_start;
    std::string m_name;
    PerfTester(std::string m_name, std::function<void(std::string, float)> callback);
public:
    
    ~PerfTester();
};

// Performance logging manager
// Handles recieving data, and logging it
// Logs once logTimeSeconds, recieves data from every PerfTester created with log
// counts average and peak execution time for each scoped test and logs it
class PerformanceLog
{
public:
    PerformanceLog(float logTimeSeconds): m_logTimeSeconds{logTimeSeconds}, m_lastLog{std::chrono::steady_clock::now()}{}
    void setLogTime(float logTime) {if (logTime > 0) m_logTimeSeconds = logTime;}
    // create RAII tester, put as an object at the start of tested scope
    PerfTester log(std::string name);
    void getData(std::string name, float delta);
    const std::unordered_map<std::string, PerfStat>& getLogData() const { return m_logData; }
    // called in mainloop
    void update();
private:
    float m_logTimeSeconds;
    std::chrono::steady_clock::time_point m_lastLog;
    std::unordered_map<std::string, PerfStat> m_logData;
};
