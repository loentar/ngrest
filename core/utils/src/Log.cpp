
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

namespace ngrest {

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
    static Log instance;
    return instance;
}

Log::Log():
    level(LogLevelInfo), verbosity(LogVerbosityDefault)
{
    const char* logFile = getenv("NGREST_LOG_FILE");
    if (!logFile) {
        stream = &std::cout;
        streamErr = &std::cerr;
    } else {
        const char* logFileAppend = getenv("NGREST_LOG_FILE_APPEND");
        static std::ofstream outStream;
        std::ios::openmode flags = std::ios::out;

        if (!!logFileAppend && (!strcmp(logFileAppend, "1") || !strcmp(logFileAppend, "TRUE")))
            flags |= std::ios::app;

        outStream.open(logFile, flags);

        if (outStream.good()) {
            stream = &outStream;
            streamErr = &outStream;
        } else {
            std::cerr << "Warning: cannot open Log file: \"" << logFile
                      << "\". using stdout/stderr..\n\n" << std::endl;
            stream = &std::cout;
            streamErr = &std::cerr;
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
        NGREST_CHECK_LOG_LEVEL(VERBOSE, Verbose)
        NGREST_CHECK_LOG_LEVEL(TRACE, Trace)
        std::cerr << "ngrest:Log: Invalid NGREST_LOG_LEVEL value: [" << logLevel
                  << "]\nSupported values are (ALERT, CRIT, ERROR, WARNING, NOTICE,"
                         "INFO, DEBUG, VERBOSE, TRACE)" << std::endl;
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
    std::ostream* out = (logLevel <= LogLevelWarning) ? streamErr : stream;

    if (!out || logLevel > level)
        return LogStream(nullptr);

    if ((verbosity & LogVerbosityLevel)) {
        switch (logLevel) {
        case LogLevelAlert:
            *out << "ALERT ";
            break;

        case LogLevelCrit:
            *out << "CRITICAL ";
            break;

        case LogLevelError:
            *out << "ERROR ";
            break;

        case LogLevelWarning:
            *out << "WARNING ";
            break;

        case LogLevelNotice:
            *out << "NOTICE ";
            break;

        case LogLevelInfo:
            *out << "INFO ";
            break;

        case LogLevelDebug:
            *out << "DEBUG ";
            break;

        case LogLevelVerbose:
            *out << "VERBOSE ";
            break;

        case LogLevelTrace:
            *out << "TRACE ";
            break;

        default:
            *out << "UNKNOWN ";
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

        *out << buff;
    }

    if ((verbosity & LogVerbosityFileLine))
        *out << baseName(fileLine);

    if ((verbosity & LogVerbosityFunction))
        *out << function << ": ";

    return LogStream(out);
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
