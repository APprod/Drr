
// #include "core/timer.hpp"
// #include "core/debug.hpp"

// Timer::Timer()
// {
// }

// void Timer::init(float millis)
// {
//     m_start = MyTime::Clock::now();
//     m_dur = std::chrono::microseconds(static_cast<int64_t>(millis * 1000));
// }

// bool Timer::check()
// {
//     auto time = MyTime::Clock::now();
//     auto dur = time - m_start;

//     auto difference = std::chrono::duration_cast<std::chrono::microseconds>(dur);
//     dbg::GetLogger().DebugInfo("dufference:",difference.count());
//     dbg::GetLogger().DebugInfo("m_dur:", m_dur.count());
//     if (difference > m_dur)
//     {
//         return true;
//     }
//     return false;
// }

// float Timer::get()
// {
//     auto time = MyTime::Clock::now();
//     auto dur = time - m_start;
//     float milliseconds = std::chrono::duration<float, std::milli>(dur).count();
//     return milliseconds;
// }

// Timer::~Timer()
// {
// }

// bool TimersSystem::createTimer(std::string name, float millis)
// {
//     if (m_map.count(name) != 0) return false;
//     m_map[name] = Timer();
//     m_map[name].init(millis);
//     return true;
// }

// bool TimersSystem::removeTimer(std::string name)
// {
//     if (m_map.count(name) == 0) return false;
//     m_map.erase(name);
//     return true;
// }

// Timer& TimersSystem::getTimer(std::string name, bool &ok)
// {
//     if (m_map.count(name) == 0)
//     {
//         m_map.emplace("badTimer", Timer());
//         m_map["badTimer"].init(0.0f);
//         ok = false;
//         return m_map["badTimer"];
//     }
//     ok = true;
//     return m_map[name];
// }
