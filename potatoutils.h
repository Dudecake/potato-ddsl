#ifndef POTATOUTILS_H
#define POTATOUTILS_H

#include <iostream>
#include <string>
#include <random>

static std::mt19937_64 rng;

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

#ifndef POTATO_UNLIKELY
#if __GNUC__ >= 3
/**
Provides optimization hint to the compiler
to optimize for the expression being false.
@param expr boolean expression.
@returns value of expression.
*/
#define POTATO_UNLIKELY(expr) __builtin_expect(expr, 0)
#else
/**
Provides optimization hint to the compiler
to optimize for the expression being false.
@param expr boolean expression.
@returns value of expression.
**/
#define POTATO_UNLIKELY(expr) expr
#endif
#endif

#ifndef POTATO_LIKELY
#if __GNUC__ >= 3
/**
Provides optimization hint to the compiler
to optimize for the expression being false.
@param expr boolean expression.
@returns value of expression.
*/
#define POTATO_LIKELY(expr) __builtin_expect(!(expr), 0)
#else
/**
Provides optimization hint to the compiler
to optimize for the expression being false.
@param expr boolean expression.
@returns value of expression.
**/
#define POTATO_LIKELY(expr) expr
#endif
#endif

inline bool endsWith(std::string &value, std::string &suffix)
{
    if (suffix.size() > value.size()) return false;
    return std::equal(suffix.rbegin(), suffix.rend(), value.rbegin());
}

inline bool endsWith(const std::string &value, const std::string &suffix)
{
    if (suffix.size() > value.size()) return false;
    return std::equal(suffix.rbegin(), suffix.rend(), value.rbegin());
}

#endif // POTATOUTILS_H
