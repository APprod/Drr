#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <cassert>
#include <memory>
#include <vector>
#include <sstream>
#include <format>

namespace dbg {

enum class Severity
{
    DBGINFO,
    INFO,
    WRN,
    ERROR,
    FATAL
};

inline const char* SeverityColor(dbg::Severity s);

inline const char* ToString(Severity s);

struct MyMessage
{
    Severity severity;
    std::string message;
};

class ISink
{
public:
    virtual ~ISink() = default;
    virtual void Write(const MyMessage& message) = 0;
};

class FileSink : public ISink
{
public:
    FileSink(const std::string& path);
    void Write(const MyMessage& message) override;

private:
    std::ofstream file;
};

class ConsoleSink : public ISink
{
public:
    ConsoleSink() = default;
    void Write(const MyMessage& message) override;
};

template<typename T> // Required since we use it in logging alternative is format
concept Streamable = requires(std::ostream& os, T a) {
    { os << a } -> std::same_as<std::ostream&>;
};

class Logger
{
public:
    static Logger& GetLogger();

    template<typename... Args> //supports <format>
    void LogFormat(Severity s, std::format_string<Args...> fmt, Args&&... args) {
        Log(s, std::format(fmt, std::forward<Args>(args)...));
    }
    template<Streamable ...Args>
    void DebugInfo(Args&& ...args) {
        LogTemplate(Severity::DBGINFO, std::forward<Args>(args)...);
    }
    template<Streamable ...Args>
    void Info(Args&& ...args) {
        LogTemplate(Severity::INFO, std::forward<Args>(args)...);
    }
    template<Streamable ...Args>
    void Warn(Args&& ...args) {
        LogTemplate(Severity::WRN, std::forward<Args>(args)...);
    }
    template<Streamable ...Args>
    void Error(Args&& ...args) {
        LogTemplate(Severity::ERROR, std::forward<Args>(args)...);
    }
    template<Streamable ...Args>
    void Fatal(Args&& ...args) {
        LogTemplate(Severity::FATAL, std::forward<Args>(args)...);
    }

    void Log(Severity s, std::string message);
    void AddSink(std::unique_ptr<ISink> sink);
    void SetMinSeverity(Severity s);

    const std::vector<MyMessage>& GetMessages() const;

private:
    Logger() = default;
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    template<Streamable ...Args>
    void LogTemplate(Severity s, Args&&... args) {
        std::ostringstream oss;
        (oss << ... << args);
        Log(s, oss.str());
    }
    std::vector<MyMessage> m_messagesLog;
    std::vector<std::unique_ptr<ISink>> m_sinks;
    Severity m_minSeverity = Severity::DBGINFO;
};

inline Logger& GetLogger() {
    return Logger::GetLogger();
}
}
