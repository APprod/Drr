#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <cassert>
#include <memory>
#include <vector>
#include <sstream>

#ifdef M_Deb
#define M_ASSERT( expected,  message, critic, quiet ) \
       if (!(expected))\
        { \
            if (!(quiet))\
            {\
                std::cerr << "Asertion failed"    << "\n";\
                std::cerr << "---------------"    << "\n";\
                std::cerr << message              << "\n";\
                std::cerr << "---------------"    << "\n";\
                std::cerr << __FILE__             << "\n";\
                std::cerr << "Line:" << __LINE__  << "\n";\
                std::cerr << "---------------"    << "\n";\
            } else{\
                std::cerr << message              << "\n";\
            }\
            if (critic == fwk::A_L::high)\
            {\
                std::abort();\
            }\
            if (critic == fwk::A_L::mid)\
            {\
                if (fwk::ex_dialog()) exit(-1);\
               \
            }\
        }
#else
#define M_ASSERT( expected,  message, critic, quiet ) 
#endif

namespace dbg {

enum class Severity
{
    DBGINFO,
    INFO,
    WRN,
    ERROR,
    FATAL
};

inline const char* ToString(Severity s) {
    switch (s) {
        case Severity::DBGINFO: return "DBGINFO";
        case Severity::INFO:   return "INFO";
        case Severity::WRN:    return "WRN";
        case Severity::ERROR:  return "ERROR";
        case Severity::FATAL:  return "FATAL";
        default:               return "UNKNOWN";
    }
}

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
    explicit FileSink(const std::string& path);
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

class Logger
{
public:
    static Logger& GetLogger();

    template<typename ...Args>
    void DebugInfo(Args&& ...args) {
        LogTemplate(Severity::DBGINFO, std::forward<Args>(args)...);
    }
    template<typename ...Args>
    void Info(Args&& ...args) {
        LogTemplate(Severity::INFO, std::forward<Args>(args)...);
    }
    template<typename ...Args>
    void Warn(Args&& ...args) {
        LogTemplate(Severity::WRN, std::forward<Args>(args)...);
    }
    template<typename ...Args>
    void Error(Args&& ...args) {
        LogTemplate(Severity::ERROR, std::forward<Args>(args)...);
    }
    template<typename ...Args>
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

    template<typename ...Args>
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
