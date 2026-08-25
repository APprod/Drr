#include "utils/log.hpp"
#include "platform.hpp"

#include <string>
#include <iostream>
#include <filesystem>

namespace mylog {

inline const char* SeverityColor(mylog::Severity s)
{
    switch (s) {
        case mylog::Severity::DBGINFO: return "\033[35m"; // violet
        // case mylog::Severity::DBGINFO: return "\033[90m"; // bright black (gray)
        case mylog::Severity::INFO:    return "\033[34m";       // blue
        case mylog::Severity::WRN:     return "\033[33m";       // yellow
        case mylog::Severity::ERROR:   return "\033[38;5;208m"; // orange
        case mylog::Severity::FATAL:   return "\033[31m";       // red
        default: return "\033[0m";
    }
}

const char* ToString(Severity s) {
    switch (s) {
        case Severity::DBGINFO: return "DBGINFO";
        case Severity::INFO:   return "MYINFO";
        case Severity::WRN:    return "WRN";
        case Severity::ERROR:  return "ERROR";
        case Severity::FATAL:  return "FATAL";
        default:               return "UNKNOWN";
    }
}

FileSink::FileSink(const std::string& path, size_t maxBytes)
    : m_maxBytes(maxBytes)
{
    std::error_code ec;
    if (std::filesystem::exists(path, ec)) {
        auto size = std::filesystem::file_size(path, ec);
        if (!ec && size > m_maxBytes) {
            auto backup = std::filesystem::path(path);
            backup += ".old";
            std::filesystem::rename(path, backup, ec);
            if (ec) {
                std::cerr << "Log: rename failed (" << ec.message() << ")\n";
            }
        }
    }
    m_file.open(path, std::ios::app);
}

void FileSink::Write(const MyMessage& message)
{
    m_file << ToString(message.severity)
         << ": " << message.message << '\n';
}

void ConsoleSink::Write(const MyMessage& message)
{
    std::cout << SeverityColor(message.severity)
              << ToString(message.severity)
              << ": " << message.message
              << "\033[0m\n" // reset color
              << std::flush; 
}

Logger& Logger::GetLogger()
{
    static Logger logger;
    return logger;
}

void Logger::Log(Severity s, std::string message)
{
    if (s < m_minSeverity)
        return;

    MyMessage msg{s, std::move(message)};
    m_messagesLog.push_back(msg);

    for (auto& sink : m_sinks)
        sink->Write(msg);
}

void Logger::AddSink(std::unique_ptr<ISink> sink)
{
    m_sinks.push_back(std::move(sink));
    m_sinks.back()->Write({Severity::INFO,
        "\n========== Log started: " + platform::CurrentDateTime() + " =========="});
}

void Logger::SetMinSeverity(Severity s)
{
    m_minSeverity = s;
}

const std::vector<MyMessage>& Logger::GetMessages() const
{
    return m_messagesLog;
}
}