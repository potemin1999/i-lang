/**
 * @author Ilya Potemin <i.potemin@innopolis.ru>
 * @author Kamilla Kryachkina <k.kryachkina@innopolis.ru>
 * @author Kamilya Timchenko <k.timchenko@innopolis.ru>
 * @author Evgeniy Bondarev <e.bondarev@innopolis.ru>
 */

#ifndef CC_PROJECT_UTILS_H
#define CC_PROJECT_UTILS_H

#include <cstring>
#define STRDUP(constCharPtr, outCharPtrPtr)                  \
    {                                                        \
        auto size = std::strlen(constCharPtr) + 1;           \
        *outCharPtrPtr = new char[size];                     \
        std::memcpy(*outCharPtrPtr, constCharPtr, size + 1); \
    }
#ifdef __BUILD_TYPE_TEST
    #define __UTIL_CHECKS_ENABLED
#endif
#ifdef __BUILD_TYPE_DEBUG
    #define __UTIL_CHECKS_ENABLED
#endif

#ifdef __UTIL_CHECKS_ENABLED
    #include <iostream>
    #define ASSERT_SHOULD_NOT_GO_HERE()                                     \
        {                                                                   \
            std::cerr << __FILE__ << ":" << std::to_string(__LINE__)        \
                      << "control flow reached invalid state" << std::endl; \
            std::exit(220);                                                 \
        }
    #define ASSERT_NOT_NULL(val)                                     \
        if ((val) == nullptr) {                                      \
            std::cerr << __FILE__ << ":" << std::to_string(__LINE__) \
                      << ": non-null assertion failed" << std::endl; \
            std::exit(220);                                          \
        }
    #define ASSERT_TRUE(val)                                               \
        if (!(val)) {                                                      \
            std::cerr << __FILE__ << ":" << std::to_string(__LINE__)       \
                      << ": should be true assertion failed" << std::endl; \
            std::exit(220);                                                \
        }
#else
    #define ASSERT_SHOULD_NOT_GO_HERE()
    #define ASSERT_NOT_NULL(val)
    #define AASSERT_TRUE(val)
#endif

enum Counter {
    SCOPE_COUNT = 0,
    VAR_COUNT = 1,
    TYPE_COUNT = 2,
};

long long GetAndIncrementUniqueCounter(Counter c);

#endif // CC_PROJECT_UTILS_H
