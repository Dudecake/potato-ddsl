#ifndef POTATLOGGER_H
#define POTATLOGGER_H

#ifdef USE_LOG4CXX
#include <log4cxx/logger.h>

typedef log4cxx::LoggerPtr Logger;

/**
Logs a message to a specified logger with the DEBUG level.

@param logger the logger to be used.
@param message the message string to log.
*/
#define POTATO_DEBUG(logger, message) LOG4CXX_DEBUG(logger, message)

/**
Logs a message to a specified logger with the TRACE level.

@param logger the logger to be used.
@param message the message string to log.
*/
#define POTATO_TRACE(logger, message) LOG4CXX_TRACE(logger, message)

/**
Logs a message to a specified logger with the INFO level.

@param logger the logger to be used.
@param message the message string to log.
*/
#define POTATO_INFO(logger, message) LOG4CXX_INFO(logger, message)
/**
Logs a message to a specified logger with the WARN level.

@param logger the logger to be used.
@param message the message string to log.
*/
#define POTATO_WARN(logger, message) LOG4CXX_WARN(logger, message)
/**
Logs a message to a specified logger with the ERROR level.

@param logger the logger to be used.
@param message the message string to log.
*/
#define POTATO_ERROR(logger, message) LOG4CXX_ERROR(logger, message)
#else
typedef int Logger;

/**
Logs a message to a specified logger with the DEBUG level.

@param logger the logger to be used.
@param message the message string to log.
*/
#define POTATO_DEBUG(logger, message) static_cast<void>(logger), static_cast<void>(message)

/**
Logs a message to a specified logger with the TRACE level.

@param logger the logger to be used.
@param message the message string to log.
*/
#define POTATO_TRACE(logger, message) static_cast<void>(logger), static_cast<void>(message)

/**
Logs a message to a specified logger with the INFO level.

@param logger the logger to be used.
@param message the message string to log.
*/
#define POTATO_INFO(logger, message) static_cast<void>(logger), static_cast<void>(message)
/**
Logs a message to a specified logger with the WARN level.

@param logger the logger to be used.
@param message the message string to log.
*/
#define POTATO_WARN(logger, message) static_cast<void>(logger), static_cast<void>(message)
/**
Logs a message to a specified logger with the ERROR level.

@param logger the logger to be used.
@param message the message string to log.
*/
#define POTATO_ERROR(logger, message) static_cast<void>(logger), static_cast<void>(message)
#endif

class PotatoLogger
{
    public:
        static inline Logger getLogger(const std::string &loggerName)
        {
            #ifdef USE_LOG4CXX
            return log4cxx::Logger::getLogger(loggerName);
            #else
            return 0;
            #endif
        }
    private:
        PotatoLogger() = delete;
};

#endif // POTATLOGGER_H
