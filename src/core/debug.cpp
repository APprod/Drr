#include "core/debug.hpp"

#include <string>
#include <iostream>
#include <cassert>

namespace fwk
{
    
    bool ex_dialog()
    {
        std::cerr << "continue? 1/0" << std::endl; 
        int res;
        std::cin >> res;
        M_ASSERT(res == 1 || res == 0, "input 1/0", fwk::A_L::low, true);
        return res;
    }

    
}

namespace dbg {

FileSink::FileSink(const std::string& path)
    : file(path, std::ios::app)
{}

void FileSink::Write(const MyMessage& message)
{
    file << ToString(message.severity)
         << ": " << message.message << '\n';
}

void ConsoleSink::Write(const MyMessage& message)
{
    std::cout << ToString(message.severity)
              << ": " << message.message << '\n';
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