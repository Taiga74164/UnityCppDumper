#include "pch.h"
#include "logger.h"

void Logger::writeLog(const char* file, int line, LogLevel level, const std::string& message)
{
    if (m_excludedLevels.find(level) != m_excludedLevels.end()) return;

    const std::lock_guard<std::mutex> lock(m_logMutex);

    std::string formattedMessage = formatLogMessage(file, line, level, message);

    // Write to console if enabled
    if (static_cast<int>(m_output) & static_cast<int>(LogOutput::Console))
    {
        writeToConsole(formattedMessage, level);
    }

    // Write to file if enabled
    if (static_cast<int>(m_output) & static_cast<int>(LogOutput::File))
    {
        writeToFile(formattedMessage);
    }
}

void Logger::attachConsole()
{
#ifdef _WIN32
    if (!m_consoleAttached)
    {
        AllocConsole();
        
        freopen_s((FILE**)stdin, "CONIN$", "r", stdin);
        freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
        freopen_s((FILE**)stderr, "CONOUT$", "w", stderr);
        SetConsoleOutputCP(CP_UTF8);
        
        // Enable virtual terminal processing for ANSI color support
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD dwMode = 0;
        if (GetConsoleMode(hOut, &dwMode))
        {
            dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            SetConsoleMode(hOut, dwMode);
        }
        
        m_consoleAttached = true;
    }
#endif
}

void Logger::detachConsole()
{
#ifdef _WIN32
    if (m_consoleAttached)
    {
        fclose(stdin);
        fclose(stdout);
        fclose(stderr);
        FreeConsole();
        m_consoleAttached = false;
    }
#endif
}

void Logger::clearConsole()
{
#ifdef _WIN32
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    if (h == INVALID_HANDLE_VALUE) return;

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (!GetConsoleScreenBufferInfo(h, &csbi)) return;

    DWORD size = csbi.dwSize.X * csbi.dwSize.Y;
    COORD coord = {0, 0};
    DWORD written;

    FillConsoleOutputCharacter(h, TEXT(' '), size, coord, &written);
    GetConsoleScreenBufferInfo(h, &csbi);
    FillConsoleOutputAttribute(h, csbi.wAttributes, size, coord, &written);
    SetConsoleCursorPosition(h, coord);
#else
    std::cout << "\033[2J\033[H" << std::flush;
#endif
}

char Logger::consoleReadKey()
{
    return std::cin.get();
}

bool Logger::prepareFileLogging(const std::string& directory)
{
    try
    {
        // Create directory if it doesn't exist
        if (!std::filesystem::exists(directory))
        {
            if (!std::filesystem::create_directories(directory))
            {
                return false;
            }
        }

        // Generate filename with timestamp
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        
        struct tm tm_buf;
#ifdef _WIN32
        gmtime_s(&tm_buf, &time_t);
#else
        gmtime_r(&time_t, &tm_buf);
#endif

        // Create filename with timestamp using simple string concatenation
        std::string filename = "log_" 
            + std::to_string(1900 + tm_buf.tm_year) + "-"
            + (tm_buf.tm_mon + 1 < 10 ? "0" : "") + std::to_string(tm_buf.tm_mon + 1) + "-"
            + (tm_buf.tm_mday < 10 ? "0" : "") + std::to_string(tm_buf.tm_mday) + "_"
            + (tm_buf.tm_hour < 10 ? "0" : "") + std::to_string(tm_buf.tm_hour) + "-"
            + (tm_buf.tm_min < 10 ? "0" : "") + std::to_string(tm_buf.tm_min) + "-"
            + (tm_buf.tm_sec < 10 ? "0" : "") + std::to_string(tm_buf.tm_sec) + ".txt";
        
        m_logFilePath = directory + "/" + filename;

        // Close existing file if open
        if (m_logFile && m_logFile->is_open())
        {
            m_logFile->close();
        }

        // Open new log file
        m_logFile = std::make_unique<std::ofstream>(m_logFilePath, std::ios::out | std::ios::app);
        if (!m_logFile->is_open())
        {
            m_logFile.reset();
            return false;
        }

        return true;
    }
    catch (const std::exception&)
    {
        return false;
    }
}

void Logger::closeFileLogging()
{
    if (m_logFile && m_logFile->is_open())
    {
        m_logFile->close();
        m_logFile.reset();
    }
}

void Logger::writeToConsole(const std::string& formattedMessage, LogLevel level)
{
#ifdef _WIN32
    if (m_enableColors)
    {
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hConsole != INVALID_HANDLE_VALUE)
        {
            CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
            if (GetConsoleScreenBufferInfo(hConsole, &consoleInfo))
            {
                WORD savedAttributes = consoleInfo.wAttributes;
                WORD levelColor = getLevelColor(level);
                WORD filenameColor = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY; // Light Blue
                WORD lineColor = FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY; // Light Yellow
                
                size_t pos = 0;
                bool foundFileSection = false;
                
                // Handle timestamp if present (starts with [HH:MM:SS])
                if (formattedMessage[0] == '[' && formattedMessage.length() > 8)
                {
                    size_t firstBracketEnd = formattedMessage.find(']');
                    if (firstBracketEnd != std::string::npos && firstBracketEnd < 12)
                    {
                        std::cout << formattedMessage.substr(0, firstBracketEnd + 2);
                        pos = firstBracketEnd + 2;
                    }
                }
                
                // Handle filename and line number
                if (pos < formattedMessage.length() && formattedMessage[pos] == '[')
                {
                    size_t sectionStart = pos;
                    size_t sectionEnd = formattedMessage.find(']', sectionStart);
                    
                    if (sectionEnd != std::string::npos)
                    {
                        std::string section = formattedMessage.substr(sectionStart + 1, sectionEnd - sectionStart - 1);
                        
                        size_t colonPos = section.find(':');
                        size_t dotPos = section.find('.');
                        
                        if (dotPos != std::string::npos && colonPos != std::string::npos && colonPos > dotPos)
                        {
                            foundFileSection = true;
                            std::cout << "[";
                            
                            // Print filename in light blue
                            SetConsoleTextAttribute(hConsole, filenameColor);
                            std::cout << section.substr(0, colonPos);
                            
                            // Print colon in default color
                            SetConsoleTextAttribute(hConsole, savedAttributes);
                            std::cout << ":";
                            
                            // Print line number in light yellow
                            SetConsoleTextAttribute(hConsole, lineColor);
                            std::cout << section.substr(colonPos + 1);
                            
                            // Reset color and close bracket
                            SetConsoleTextAttribute(hConsole, savedAttributes);
                            std::cout << "] ";
                            
                            pos = sectionEnd + 2; // move past "] "
                        }
                    }
                }
                
                if (!foundFileSection)
                {
                    // Find the next bracket which should be the level
                    size_t nextBracket = formattedMessage.find('[', pos);
                    if (nextBracket != std::string::npos)
                    {
                        std::cout << formattedMessage.substr(pos, nextBracket - pos);
                        pos = nextBracket;
                    }
                }
                
                // Handle log level [LEVEL]
                if (pos < formattedMessage.length() && formattedMessage[pos] == '[')
                {
                    size_t levelStart = pos + 1;
                    size_t levelEnd = formattedMessage.find(']', levelStart);
                    
                    if (levelEnd != std::string::npos)
                    {
                        std::cout << "[";
                        
                        // Print level in its color
                        SetConsoleTextAttribute(hConsole, levelColor);
                        std::cout << formattedMessage.substr(levelStart, levelEnd - levelStart);
                        
                        // Reset color and print rest of message
                        SetConsoleTextAttribute(hConsole, savedAttributes);
                        std::cout << formattedMessage.substr(levelEnd) << std::endl;
                        return;
                    }
                }
                
                SetConsoleTextAttribute(hConsole, savedAttributes);
                std::cout << formattedMessage.substr(pos) << std::endl;
                return;
            }
        }
    }
#endif
    std::cout << formattedMessage << std::endl;
}

void Logger::writeToFile(const std::string& formattedMessage)
{
    if (m_logFile && m_logFile->is_open())
    {
        *m_logFile << formattedMessage << std::endl;
        m_logFile->flush();
    }
}

std::string Logger::formatLogMessage(const char* file, int line, LogLevel level, const std::string& message)
{
    std::string result;

    // Add timestamp if enabled
    if (m_showTimeStamp)
    {
        result += "[" + getCurrentTimeString() + "] ";
    }

    // Add file info if enabled
    if (m_showFileName && file && std::strlen(file) > 0)
    {
        std::string filename = std::filesystem::path(file).filename().string();
        result += "[" + filename;
        
        if (m_showLineNumber && line > 0)
        {
            result += ":" + std::to_string(line);
        }
        
        result += "] ";
    }

    // Add level
    result += "[" + getLevelString(level) + "] ";

    // Add message
    result += message;

    return result;
}

std::string Logger::getLevelString(LogLevel level)
{
    switch (level)
    {
        case LogLevel::Debug:   return "DEBUG";
        case LogLevel::Info:    return "INFO";
        case LogLevel::Warning: return "WARN";
        case LogLevel::Error:   return "ERROR";
        default:                return "LOG";
    }
}

std::string Logger::getCurrentTimeString()
{
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    
    struct tm tm_buf;
#ifdef _WIN32
    localtime_s(&tm_buf, &time_t);
#else
    localtime_r(&time_t, &tm_buf);
#endif

    // Format time using simple string concatenation
    std::string timeStr = 
        (tm_buf.tm_hour < 10 ? "0" : "") + std::to_string(tm_buf.tm_hour) + ":" +
        (tm_buf.tm_min < 10 ? "0" : "") + std::to_string(tm_buf.tm_min) + ":" +
        (tm_buf.tm_sec < 10 ? "0" : "") + std::to_string(tm_buf.tm_sec);
    
    return timeStr;
}

#ifdef _WIN32
WORD Logger::getLevelColor(LogLevel level)
{
    switch (level)
    {
        case LogLevel::Debug:
            return FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_INTENSITY; // Magenta
        case LogLevel::Info:
            return FOREGROUND_GREEN | FOREGROUND_INTENSITY; // Green
        case LogLevel::Warning:
            return FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY; // Yellow
        case LogLevel::Error:
            return FOREGROUND_RED | FOREGROUND_INTENSITY; // Red
        default:
            return FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY; // White
    }
}
#endif