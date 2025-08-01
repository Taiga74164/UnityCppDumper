#pragma once

#include <string>
#include <memory>
#include <format>
#include <fstream>
#include <mutex>
#include <unordered_set>

#ifdef _WIN32
#include <Windows.h>
#endif

#define LOG(fmt, ...)       Logger::log(__FILE__, __LINE__, LogLevel::Info, fmt, __VA_ARGS__)
#define LOG_INFO(fmt, ...)  Logger::log(__FILE__, __LINE__, LogLevel::Info, fmt, __VA_ARGS__)
#define LOG_DEBUG(fmt, ...) Logger::log(__FILE__, __LINE__, LogLevel::Debug, fmt, __VA_ARGS__)
#define LOG_ERROR(fmt, ...) Logger::log(__FILE__, __LINE__, LogLevel::Error, fmt, __VA_ARGS__)
#define LOG_WARNING(fmt, ...)  Logger::log(__FILE__, __LINE__, LogLevel::Warning, fmt, __VA_ARGS__)

enum class LogLevel
{
    Debug,
    Info,
    Warning,
    Error
};

enum class LogOutput
{
    Console = 1,
    File = 2,
    Both = Console | File
};

class Logger
{
public:
    static Logger& instance()
    {
        static Logger logger;
        return logger;
    }

    static Logger& attach()
    {
        instance().attachConsole();
        return instance();
    }

    Logger& showFileName(bool show = true)
    {
        m_showFileName = show;
        return *this;
    }

    Logger& showLineNumber(bool show = true)
    {
        m_showLineNumber = show;
        return *this;
    }

    Logger& showTimeStamp(bool show = true)
    {
        m_showTimeStamp = show;
        return *this;
    }

    Logger& logToFile(const std::string& directory = "logs")
    {
        prepareFileLogging(directory);
        m_output = LogOutput::Both;
        return *this;
    }

    Logger& consoleOnly()
    {
        m_output = LogOutput::Console;
        return *this;
    }

    Logger& fileOnly()
    {
        m_output = LogOutput::File;
        return *this;
    }

    Logger& exclude(LogLevel level)
    {
        m_excludedLevels.insert(level);
        return *this;
    }

    Logger& include(LogLevel level)
    {
        m_excludedLevels.erase(level);
        return *this;
    }

    Logger& clearExclusions()
    {
        m_excludedLevels.clear();
        return *this;
    }

    Logger& enableColors(bool enable = true)
    {
        m_enableColors = enable;
        return *this;
    }

    template<typename... Args>
    static void log(const char* file, int line, LogLevel level, const std::string& fmt, Args&&... args)
    {
        if constexpr (sizeof...(args) == 0)
        {
            instance().writeLog(file, line, level, fmt);
        }
        else
        {
            try
            {
                std::string formatted = std::vformat(fmt, std::make_format_args(args...));
                instance().writeLog(file, line, level, formatted);
            }
            catch (const std::exception&)
            {
                instance().writeLog(file, line, level, fmt + " [FORMAT ERROR]");
            }
        }
    }

    // Direct logging methods
    template<typename... Args>
    static void info(const std::string& fmt, Args&&... args)
    {
        if constexpr (sizeof...(args) == 0)
        {
            instance().writeLog("", 0, LogLevel::Info, fmt);
        }
        else
        {
            try
            {
                std::string formatted = std::vformat(fmt, std::make_format_args(args...));
                instance().writeLog("", 0, LogLevel::Info, formatted);
            }
            catch (const std::exception&)
            {
                instance().writeLog("", 0, LogLevel::Info, fmt + " [FORMAT ERROR]");
            }
        }
    }

    template<typename... Args>
    static void debug(const std::string& fmt, Args&&... args)
    {
        if constexpr (sizeof...(args) == 0)
        {
            instance().writeLog("", 0, LogLevel::Debug, fmt);
        }
        else
        {
            try
            {
                std::string formatted = std::vformat(fmt, std::make_format_args(args...));
                instance().writeLog("", 0, LogLevel::Debug, formatted);
            }
            catch (const std::exception&)
            {
                instance().writeLog("", 0, LogLevel::Debug, fmt + " [FORMAT ERROR]");
            }
        }
    }

    template<typename... Args>
    static void error(const std::string& fmt, Args&&... args)
    {
        if constexpr (sizeof...(args) == 0)
        {
            instance().writeLog("", 0, LogLevel::Error, fmt);
        }
        else
        {
            try
            {
                std::string formatted = std::vformat(fmt, std::make_format_args(args...));
                instance().writeLog("", 0, LogLevel::Error, formatted);
            }
            catch (const std::exception&)
            {
                instance().writeLog("", 0, LogLevel::Error, fmt + " [FORMAT ERROR]");
            }
        }
    }

    template<typename... Args>
    static void warn(const std::string& fmt, Args&&... args)
    {
        if constexpr (sizeof...(args) == 0)
        {
            instance().writeLog("", 0, LogLevel::Warning, fmt);
        }
        else
        {
            try
            {
                std::string formatted = std::vformat(fmt, std::make_format_args(args...));
                instance().writeLog("", 0, LogLevel::Warning, formatted);
            }
            catch (const std::exception&)
            {
                instance().writeLog("", 0, LogLevel::Warning, fmt + " [FORMAT ERROR]");
            }
        }
    }

    static void detach()
    {
        instance().detachConsole();
    }

    static void clear()
    {
        instance().clearConsole();
    }

    static char readKey()
    {
        return instance().consoleReadKey();
    }

    static void close()
    {
        instance().closeFileLogging();
    }

private:
    Logger() = default;
    ~Logger()
    {
        closeFileLogging();
        detachConsole();
    }

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    void writeLog(const char* file, int line, LogLevel level, const std::string& message);
    void attachConsole();
    void detachConsole();
    void clearConsole();
    char consoleReadKey();
    bool prepareFileLogging(const std::string& directory);
    void closeFileLogging();
    void writeToConsole(const std::string& formattedMessage, LogLevel level);
    void writeToFile(const std::string& formattedMessage);
    std::string formatLogMessage(const char* file, int line, LogLevel level, const std::string& message);
    std::string getLevelString(LogLevel level);
    std::string getCurrentTimeString();

#ifdef _WIN32
    WORD getLevelColor(LogLevel level);
#endif

    // Configuration
    bool m_showFileName = true;
    bool m_showLineNumber = true;
    bool m_showTimeStamp = false;
    bool m_enableColors = true;
    LogOutput m_output = LogOutput::Console;
    std::unordered_set<LogLevel> m_excludedLevels;

    // File logging
    std::string m_logFilePath;
    std::unique_ptr<std::ofstream> m_logFile;

    // Thread safety
    std::mutex m_logMutex;
    bool m_consoleAttached = false;
};