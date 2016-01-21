#ifndef NGREST_UTILS_LOG_H
#define NGREST_UTILS_LOG_H

#include "ngrestutilsexport.h"
#include "LogStream.h"

#ifndef NGREST_FILE_LINE
#define NGREST_STRINGIZE(x) #x
#define NGREST_TO_STRING(x) NGREST_STRINGIZE(x)
#define NGREST_FILE_LINE __FILE__ ":" NGREST_TO_STRING(__LINE__) " "
#endif

namespace ngrest {

//! put data to log
#define NGREST_LOG_WRITE(NGREST_LOG_LEVEL)\
    Log::inst().write(::ngrest::Log::NGREST_LOG_LEVEL, NGREST_FILE_LINE, __FUNCTION__)

//! log alert message
#define LogAlert()   NGREST_LOG_WRITE(LogLevelAlert)

//! log critical message
#define LogCrit()    NGREST_LOG_WRITE(LogLevelCrit)

//! log error message
#define LogError()   NGREST_LOG_WRITE(LogLevelError)

//! log warning message
#define LogWarning() NGREST_LOG_WRITE(LogLevelWarning)

//! log notice message
#define LogNotice()  NGREST_LOG_WRITE(LogLevelNotice)

//! log info message
#define LogInfo()    NGREST_LOG_WRITE(LogLevelInfo)

//! log debug message
#define LogDebug()   NGREST_LOG_WRITE(LogLevelDebug)

//! log verbose message
#define LogVerbose()  NGREST_LOG_WRITE(LogLevelVerbose)

//! log trace message
#define LogTrace()  NGREST_LOG_WRITE(LogLevelTrace)

//! log standard debug message
#define LogLabel()   NGREST_LOG_WRITE(LogLevelDebug) << "*";


//! log enter/exit function message
#define LogEntry() \
    LogEntryScope logEntryScope(NGREST_FILE_LINE, __FUNCTION__);

//! log enter/exit function message with given level
#define LogEntryL(NGREST_LOG_LEVEL) \
    LogEntryScope logEntryScope(NGREST_FILE_LINE, __FUNCTION__, NGREST_LOG_LEVEL);


//! Logger class
/*! example:
        LogDebug() << "Some value = " << value;

      there is an log macro list:
      - LogAlert()    - log alert message
      - LogCrit()     - log critical message
      - LogError()    - log error message
      - LogWarning()  - log warning message
      - LogNotice()   - log notice message
      - LogInfo()     - log info message
      - LogDebug()    - log high level debug message
      - LogVerbose()  - log verbose message
      - LogTrace()    - log trace message
      - LogLabel()    - log standard debug message
      - LogEntry()    - log enter/exit scope message
  */
class NGREST_UTILS_EXPORT Log
{
public:
    enum LogLevel  //! log level
    {
        LogLevelAlert,    //!< alert
        LogLevelCrit,     //!< critical
        LogLevelError,    //!< error
        LogLevelWarning,  //!< warning
        LogLevelNotice,   //!< notice
        LogLevelInfo,     //!< information
        LogLevelDebug,    //!< debug message
        LogLevelVerbose,  //!< verbose message
        LogLevelTrace     //!< trace message
    };

    enum LogVerbosity      //! log verbosity
    {
        LogVerbosityText      = 0,        //!< log text only
        LogVerbosityLevel     = (1 << 0), //!< message level (ERROR, WARNING, etc.)
        LogVerbosityFileLine  = (1 << 1), //!< source file name and file line
        LogVerbosityFunction  = (1 << 2), //!< function name
        LogVerbosityDateTime  = (1 << 3), //!< date and time

        LogVerbosityDefault   = LogVerbosityLevel | //!< default
        LogVerbosityFileLine | LogVerbosityFunction | LogVerbosityDateTime,

        LogVerbosityAll       = LogVerbosityLevel | //!< all
        LogVerbosityFileLine | LogVerbosityFunction | LogVerbosityDateTime
    };

public:
    //! get log instance
    /*! \return log instance
      */
    static Log& inst();

    //! set log stream to output messages
    /*! \param  outStream - log stream (nullptr - do not output)
    */
    void setLogStream(std::ostream* outStream);

    //! set log level
    /*! log messages with level equal or higher that set
        example: disable debug and info outputting.
                 output only warnings and more important messages
          SetLogLevel(::Log::LogLevelWarning)

        \param  logLevel - log level
    */
    void setLogLevel(LogLevel logLevel);

    //! set log verbosity
    /*! example: log text only
        setLogVerbosity(Log::LogVerbosityText)
        \param  logVerbosity - log verbosity
    */
    void setLogVerbosity(int logVerbosity);

    //! log message
    /*! \param  logLevel - message log level
        \param  fileLine - source file name and line
        \param  function - function
        \return log stream
    */
    LogStream write(LogLevel logLevel, const char* fileLine, const char* function);

private:
    Log();
    ~Log();
    Log(const Log&);
    Log& operator=(const Log&);

private:
    std::ostream*  stream;    //!< output stream
    std::ostream*  streamErr; //!< output stream for warning and higher levels
    LogLevel       level;     //!< log level
    int            verbosity; //!< verbosity
};


//! log entry
class NGREST_UTILS_EXPORT LogEntryScope
{
public:
    //! log entry message
    /*! \param  fileLine - source file name and line number
        \param  function - function
        \param  eLevel - level
      */
    LogEntryScope(const char* fileLine, const char* function,
                  Log::LogLevel eLevel = defaultLevel);

    //! destructor
    ~LogEntryScope();

    //! set default log level for log entry
    /*! \param eLevel - default log level
    */
    static void setDefaultLogLevel(Log::LogLevel level = Log::LogLevelDebug);

private:
    static Log::LogLevel  defaultLevel; //!< default log level
    Log::LogLevel         level;        //!< current log level
    const char*           fileLine;    //!< file name and line number
    const char*           function;    //!< function
};


//! log success message
NGREST_UTILS_EXPORT std::ostream& logResultSuccess(std::ostream& stream);

//! log done message
NGREST_UTILS_EXPORT std::ostream& logResultDone(std::ostream& stream);

//! log warning message
NGREST_UTILS_EXPORT std::ostream& logResultWarning(std::ostream& stream);

//! log fail message
NGREST_UTILS_EXPORT std::ostream& logResultFailed(std::ostream& stream);

}

#endif // NGREST_UTILS_LOG_H
