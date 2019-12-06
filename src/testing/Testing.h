/**
 * Created by Ilya Potemin on 10/3/19.
 */

#ifndef MODULE_TESTING_H
#define MODULE_TESTING_H

enum TestingEvent { START_TEST, END_TEST };

class Testing;

typedef void (*TestFunc)();

class __attribute__((unused)) TestingInternal;

typedef void (*TestEventCallback)(Testing *t, TestingEvent event);

bool IsSTLInitialized();

extern void (*allocatorStart)();
extern void (*allocatorEnd)();

class Testing {
    friend TestingInternal;

  public:
    static void registerTest(const char *testName, TestFunc testFunc);

    static bool reportWarning(char *&&warning);

    static bool attachTestEventCallback(TestEventCallback callback);

    static bool detachTestEventCallback(TestEventCallback callback);

    static void sleep(long ms);

    static void fail();

    static void pass();
};

#ifdef __BUILD_TYPE_TEST
    #define UNIT_TEST(TestName)                                         \
        void Test##TestName();                                          \
        __attribute__((constructor(230))) void __InitTest##TestName() { \
            Testing::registerTest(#TestName, Test##TestName);           \
        }                                                               \
        __attribute_used__ void Test##TestName()
#else
    #define UNIT_TEST(TestName) __attribute_used__ void Test##TestName()
#endif

#endif // MODULE_TESTING_H
