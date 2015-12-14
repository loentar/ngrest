
#include <time.h>
#include <sys/timeb.h>
#include <stdlib.h>
#include <string.h>
#ifdef WIN32
#include <windows.h>
#endif
#include <iostream>
#include <fstream>
#include <string>
#include "tocstring.h"
#include "Log.h"

namespace ngrest
{

inline const char* baseName(const char* path)
{
    const char* last = path;
    while (*path) {
        if (*path == '/' || *path == '\\')
            last = path + 1;
        ++path;
    }
    return last;
}

Log& Log::inst()
{
    static Log tInst;
    return tInst;
}

Log::Log():
    level(LogLevelInfo), verbosity(LogVerbosityDefault)
{
    const char* logFile = getenv("NGREST_LOG_FILE");
    if (!logFile) {
        stream = &std::cout;
    } else {
        const char* logFileAppend = getenv("NGREST_LOG_FILE_APPEND");
        static std::ofstream outStream;
        std::ios::openmode flags = std::ios::out;

        if (!!logFileAppend && (!strcmp(logFileAppend, "1") || !strcmp(logFileAppend, "TRUE")))
            flags |= std::ios::app;

        outStream.open(logFile, flags);

        if (outStream.good()) {
            stream = &outStream;
        } else {
            std::cerr << "Warning: cannot open Log file: \"" << logFile
                      << "\". using stdout..\n\n" << std::endl;
            stream = &std::cout;
        }
    }

    const char* logLevel = getenv("NGREST_LOG_LEVEL");
    if (logLevel) {
#define NGREST_CHECK_LOG_LEVEL(NGREST_LOG_LEVEL, NGREST_LOG_LEVEL_ENUM)\
        if (!strcmp(logLevel, #NGREST_LOG_LEVEL)) {\
            level = LogLevel##NGREST_LOG_LEVEL_ENUM;\
        } else

        NGREST_CHECK_LOG_LEVEL(ALERT, Alert)
        NGREST_CHECK_LOG_LEVEL(CRIT, Crit)
        NGREST_CHECK_LOG_LEVEL(ERROR, Error)
        NGREST_CHECK_LOG_LEVEL(WARNING, Warning)
        NGREST_CHECK_LOG_LEVEL(NOTICE, Notice)
        NGREST_CHECK_LOG_LEVEL(INFO, Info)
        NGREST_CHECK_LOG_LEVEL(DEBUG, Debug)
        NGREST_CHECK_LOG_LEVEL(DEBUG1, Debug1)
        NGREST_CHECK_LOG_LEVEL(DEBUG2, Debug2)
        NGREST_CHECK_LOG_LEVEL(DEBUG3, Debug3)
        std::cerr << "ngrest:Log: Invalid NGREST_LOG_LEVEL value: [" << logLevel
                  << "]\nSupported values are (ALERT, CRIT, ERROR, WARNING, NOTICE,"
                         "INFO, DEBUG, DEBUG1, DEBUG2, DEBUG3)" << std::endl;
#undef NGREST_CHECK_LOG_LEVEL
    }

    const char* logVerbosity = getenv("NGREST_LOG_VERBOSITY");
    if (logVerbosity) {
        verbosity = 0;
#define NGREST_CHECK_LOG_VERBOSITY(NGREST_LOG_VERBOSITY, NGREST_LOG_VERBOSITY_ENUM)\
        if (strstr(logVerbosity, #NGREST_LOG_VERBOSITY)) {\
            verbosity |= LogVerbosity##NGREST_LOG_VERBOSITY_ENUM;\
        }

        NGREST_CHECK_LOG_VERBOSITY(TEXT, Text)
        NGREST_CHECK_LOG_VERBOSITY(LEVEL, Level)
        NGREST_CHECK_LOG_VERBOSITY(FILELINE, FileLine)
        NGREST_CHECK_LOG_VERBOSITY(FUNCTION, Function)
        NGREST_CHECK_LOG_VERBOSITY(DATETIME, DateTime)
        NGREST_CHECK_LOG_VERBOSITY(DEFAULT, Default)
        NGREST_CHECK_LOG_VERBOSITY(ALL, All)

        if (!verbosity) {
            std::cerr << "ngrest:Log: Invalid NGREST_LOG_VERBOSITY value: [" << logVerbosity
                      << "]\nSupported values are (TEXT, LEVEL, FILELINE, FUNCTION, "
                         "DATETIME, PID, THREAD, DEFAULT, ALL). Setting default." << std::endl;
            verbosity = LogVerbosityDefault;
        }
#undef NGREST_CHECK_LOG_VERBOSITY
    }
}

Log::~Log()
{
}

void Log::setLogStream(std::ostream* outStream)
{
    stream = outStream;
}

LogStream Log::write(LogLevel logLevel, const char* fileLine, const char* function)
{
    if (!stream || logLevel > level)
        return LogStream(NULL);

    if ((verbosity & LogVerbosityLevel)) {
        switch (logLevel)
        {
        case LogLevelAlert:
            *stream << "ALERT ";
            break;

        case LogLevelCrit:
            *stream << "CRITICAL ";
            break;

        case LogLevelError:
            *stream << "ERROR ";
            break;

        case LogLevelWarning:
            *stream << "WARNING ";
            break;

        case LogLevelNotice:
            *stream << "NOTICE ";
            break;

        case LogLevelInfo:
            *stream << "INFO ";
            break;

        case LogLevelDebug:
            *stream << "DEBUG ";
            break;

        case LogLevelDebug1:
            *stream << "DEBUG1 ";
            break;

        case LogLevelDebug2:
            *stream << "DEBUG2 ";
            break;

        case LogLevelDebug3:
            *stream << "DEBUG3 ";
            break;

        default:
            *stream << "UNKNOWN ";
        }
    }

    if ((verbosity & LogVerbosityDateTime)) {
        static const int buffSize = 32;
        char buff[buffSize];

#if defined WIN32
        SYSTEMTIME sysTime;
        GetLocalTime(&sysTime);
        ngrest_snprintf(buff, buffSize, "%02d-%02d-%02d %02d:%02d:%02d.%03d ",
                        sysTime.wDay, sysTime.wMonth, sysTime.wYear,
                        sysTime.wHour, sysTime.wMinute, sysTime.wSecond, sysTime.wMilliseconds);
#else
        struct tm localTime;
        struct timeb timeBuf;
        ftime(&timeBuf);
        localtime_r(&timeBuf.time, &localTime);

        ngrest_snprintf(buff, buffSize, "%02d-%02d-%02d %02d:%02d:%02d.%03d ",
                        localTime.tm_mday, localTime.tm_mon + 1, localTime.tm_year + 1900,
                        localTime.tm_hour, localTime.tm_min, localTime.tm_sec, timeBuf.millitm);
#endif

        *stream << buff;
    }

    if ((verbosity & LogVerbosityFileLine))
        *stream << baseName(fileLine);

    if ((verbosity & LogVerbosityFunction))
        *stream << function << ": ";

    return LogStream(stream);
}

void Log::setLogLevel(LogLevel logLevel)
{
    level = logLevel;
}

void Log::setLogVerbosity(int logVerbosity)
{
    verbosity = logVerbosity;
}


Log::LogLevel LogEntryScope::defaultLevel = Log::LogLevelDebug;

LogEntryScope::LogEntryScope(const char* fileLine_, const char* function_, Log::LogLevel level_):
    level(level_),
    fileLine(fileLine_),
    function(function_)
{
    Log::inst().write(level, fileLine_, function_) << "Enter";
}

LogEntryScope::~LogEntryScope()
{
    Log::inst().write(level, fileLine, function) << "Exit";
}

void LogEntryScope::setDefaultLogLevel(Log::LogLevel level)
{
    defaultLevel = level;
}


std::ostream& logResultSuccess(std::ostream& stream)
{
    return stream << ": " << "Success";
}

std::ostream& logResultDone(std::ostream& stream)
{
    return stream << ": " << "Done";
}

std::ostream& logResultWarning(std::ostream& stream)
{
    return stream << ": " << "Warning";
}

std::ostream& logResultFailed(std::ostream& stream)
{
    return stream << ": " << "Failed";
}

}
