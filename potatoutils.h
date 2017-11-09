#ifndef POTATOUTILS_H
#define POTATOUTILS_H

#include <iostream>
#include <string>
#include <log4cxx/logger.h>
#include <ddsl.hpp>

#define HANDLE_DDSL_ERROR catch (const double &e) {\
LOG4CXX_ERROR(logger, "double" << e);\
} catch (int &e) {\
LOG4CXX_ERROR(logger, "int" << e);\
} catch (float &e) {\
LOG4CXX_ERROR(logger, "float" << e);\
} catch (const std::string &e) {\
LOG4CXX_ERROR(logger, "string" << e);\
} catch (const std::exception &e) {\
LOG4CXX_ERROR(logger, e.what());\
}

inline bool endsWith(std::string &value, std::string &suffix)
{
    if (suffix.size() > value.size()) return false;
    return std::equal(suffix.rbegin(), suffix.rend(), value.begin());
}

inline bool endsWith(const std::string &value, const std::string &suffix)
{
    if (suffix.size() > value.size()) return false;
    return std::equal(suffix.rbegin(), suffix.rend(), value.rbegin());
}

#endif // POTATOUTILS_H
