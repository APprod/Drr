#include "utils/perf.hpp"
#include "utils/log.hpp"

Tester::Tester(std::string name, float thresholdMs, bool active): 
        m_start(std::chrono::steady_clock::now()), 
        m_name(name), 
        m_thresholdMs(thresholdMs),
        m_active(active)
{
}

Tester::~Tester()
{
    if (!m_active) return;
    auto end = std::chrono::steady_clock::now();
    auto ms = std::chrono::duration<float, std::milli>(end - m_start).count();
    if (ms > m_thresholdMs * 2) mylog::GetLogger().DebugInfo("Tester " + m_name + " , ms:", ms);
    else if (ms > m_thresholdMs) mylog::GetLogger().DebugInfo("Tester: " + m_name + " , ms:", ms);
}

PerfTester::PerfTester(std::string name, std::function<void(std::string, float)> callback):
        m_callback(callback),
        m_start(std::chrono::steady_clock::now()), 
        m_name(name)
{
}

PerfTester::~PerfTester()
{
    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<float, std::milli> dur = end - m_start;
    m_callback(m_name, dur.count());
}

PerfTester PerformanceLog::log(std::string name){
    auto callback = [this](std::string testerName, float delta){getData(testerName, delta);};
    return PerfTester(name, callback);
}
void PerformanceLog::getData(std::string name, float delta){
    PerfStat& stat = m_logData[name]; //creates automatically on first access
    stat.deltas.push_back(delta);
}
void PerformanceLog::update(){
    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<float, std::milli> dur = end - m_lastLog;
    float ms = dur.count();
    if (ms > m_logTimeSeconds * 1000.0f){
        mylog::GetLogger().LogFormat(mylog::Severity::DBGINFO, "---------------------- Peformance LOG:");
        for (auto& [name, perf]: m_logData){
            auto peak = perf.peak();
            auto average = perf.average();
            mylog::GetLogger().LogFormat(mylog::Severity::DBGINFO, "{:<25} : avg: {:>8.4f}ms, peak:{:>8.4f}ms", name, average, peak);
            perf.deltas.clear();
        }
        m_lastLog = end;
    }
}
