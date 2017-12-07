#ifndef POTATLOGGER_H
#define POTATLOGGER_H

#if defined(USE_GLOG)

#include <iostream>
#include <glog/logging.h>

typedef std::ostream& Logger;

/**
Logs a message to a specified logger with the DEBUG level.

@param logger the logger to be used.
@param message the message string to log.
*/
#define POTATO_DEBUG(logger, message) static_cast<void>(logger), LOG(INFO) << message

/**
Logs a message to a specified logger with the TRACE level.

@param logger the logger to be used.
@param message the message string to log.
*/
#define POTATO_TRACE(logger, message) static_cast<void>(logger), std::stringstream() << message

/**
Logs a message to a specified logger with the INFO level.

@param logger the logger to be used.
@param message the message string to log.
*/
#define POTATO_INFO(logger, message) static_cast<void>(logger), LOG(INFO) << message
/**
Logs a message to a specified logger with the WARN level.

@param logger the logger to be used.
@param message the message string to log.
*/
#define POTATO_WARN(logger, message) static_cast<void>(logger), LOG(WARNING) << message
/**
Logs a message to a specified logger with the ERROR level.

@param logger the logger to be used.
@param message the message string to log.
*/
#define POTATO_ERROR(logger, message) static_cast<void>(logger), LOG(ERROR) << message

#elif defined(USE_LOG4CXX)
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
#include <iostream>

typedef std::ostream& Logger;

/**
Logs a message to a specified logger with the DEBUG level.

@param logger the logger to be used.
@param message the message string to log.
*/
#define POTATO_DEBUG(logger, message) static_cast<void>(logger), std::stringstream() << message

/**
Logs a message to a specified logger with the TRACE level.

@param logger the logger to be used.
@param message the message string to log.
*/
#define POTATO_TRACE(logger, message) static_cast<void>(logger), std::stringstream() << message

/**
Logs a message to a specified logger with the INFO level.

@param logger the logger to be used.
@param message the message string to log.
*/
#define POTATO_INFO(logger, message) logger << message
/**
Logs a message to a specified logger with the WARN level.

@param logger the logger to be used.
@param message the message string to log.
*/
#define POTATO_WARN(logger, message) logger << message
/**
Logs a message to a specified logger with the ERROR level.

@param logger the logger to be used.
@param message the message string to log.
*/
#define POTATO_ERROR(logger, message) logger << message
#endif

class PotatoLogger
{
    public:
        static inline Logger getLogger(const std::string &loggerName)
        {
            #ifdef USE_GLOG
               return std::cout;
            #elif defined(USE_LOG4CXX)
            return log4cxx::Logger::getLogger(loggerName);
            #else
            return std::cout;
            #endif
        }
    private:
        PotatoLogger() = delete;
};

#endif // POTATLOGGER_H
