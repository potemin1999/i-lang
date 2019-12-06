/**
 * Created by Ilya Potemin on 10/3/19.
 */

#include "Testing.h"
#include <chrono>
#include <iomanip>
#include <iostream>
#include <list>
#include <thread>

void (*allocatorStart)() = nullptr;
void (*allocatorEnd)() = nullptr;

struct TestItem {
    int index;
    int result;
    double time;
    const char *testName;
    TestFunc testFunc;
    std::list<char *> *warnings;

    TestItem *next;

    const bool operator<(const TestItem &other) const { return testName < other.testName; }
};

class FailException : std::exception {};

class PassException : std::exception {};

class TestingInternal {
  public:
    static void runTests();

    static void printResults();
};

int totalCount;
int passedCount;
int failedCount;
double totalTime;

int indexCounter;
Testing testingInstance;
TestItem *currentTest = nullptr;
// std::list<TestItem> testingQueue;
std::list<TestEventCallback> eventCallbackList;
TestItem *firstTestItem = nullptr;
TestItem *lastTestItem = nullptr;

void Testing::registerTest(const char *testName, TestFunc testFunc) {
    void *testItemRaw = std::malloc(sizeof(TestItem));
    TestItem *testItem = static_cast<TestItem *>(testItemRaw);
    testItem->index = indexCounter;
    testItem->result = 0;
    testItem->time = 0.0;
    testItem->testName = testName;
    testItem->testFunc = testFunc;
    testItem->next = nullptr;
    /* TestItem testItem = {.index = indexCounter++,
                             .result = 0,
                             .time = 0.0,
                             .testName = testName,
                             .testFunc = testFunc});*/
    if (firstTestItem == nullptr) {
        firstTestItem = testItem;
        lastTestItem = testItem;
    } else {
        lastTestItem->next = testItem;
        lastTestItem = testItem;
    }
}

bool Testing::reportWarning(char *&&warning) {
    if (currentTest == nullptr) {
        return false;
    }
    currentTest->warnings->push_back(warning);
    return true;
}

void Testing::sleep(long ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

void Testing::pass() {
    totalCount++;
    passedCount++;
    if (currentTest != nullptr) {
        currentTest->result = 0;
    }
    throw PassException();
}

void Testing::fail() {
    totalCount++;
    failedCount++;
    if (currentTest != nullptr) {
        currentTest->result = 1;
    }
    throw FailException();
}

bool Testing::attachTestEventCallback(TestEventCallback callback) {
    eventCallbackList.push_back(callback);
    return false;
}

bool Testing::detachTestEventCallback(TestEventCallback callback) {
    eventCallbackList.remove(callback);
    return false;
}

void TestingInternal::runTests() {
    // testingQueue.sort();
    auto start = std::chrono::high_resolution_clock::now();
    currentTest = firstTestItem;
    while (currentTest) {
        for (TestEventCallback callback : eventCallbackList) {
            callback(&testingInstance, TestingEvent::START_TEST);
        }
        auto testStart = std::chrono::high_resolution_clock::now();
        try {
            currentTest->testFunc();
            testingInstance.pass();
        } catch (PassException &passException) {
        } catch (FailException &failException) {
        } catch (std::exception &any) {
            totalCount++;
            failedCount++;
        }
        auto testEnd = std::chrono::high_resolution_clock::now();
        auto testTime = testEnd - testStart;
        currentTest->time = std::chrono::duration_cast<std::chrono::nanoseconds>(testTime).count();
        for (TestEventCallback callback : eventCallbackList) {
            callback(&testingInstance, TestingEvent::END_TEST);
        }
        currentTest = currentTest->next;
    }
    auto end = std::chrono::high_resolution_clock::now();
    totalTime = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
}

void TestingInternal::printResults() {
    const auto msMultiplier = 0.000001;
    std::cout << "\033[1m" << std::fixed << std::setprecision(3);
    std::cout << "Tests run    : " << totalCount << std::endl;
    std::cout << "Tests passed : " << passedCount << std::endl;
    std::cout << "Tests failed : " << failedCount << std::endl;
    std::cout << "Total time   : " << totalTime * msMultiplier << "ms" << std::endl;
    std::cout << "-------------"
              << "\033[0m" << std::endl;
    TestItem *item = firstTestItem;
    while (item) {
        auto startColor = static_cast<const char *>((item->result == 0) ? "\033[32m" : "\033[31m");
        auto status = static_cast<const char *>((item->result == 0) ? "passed" : "failed");
        std::cout << startColor << " * " << item->testName << " " << status << " in "
                  << item->time * msMultiplier << " ms\033[0m";
        if (item->warnings->empty()) {
            std::cout << std::endl;
            item = item->next;
            continue;
        } else {
            auto warnCount = item->warnings->size();
            std::cout << " : " << warnCount << (warnCount == 1 ? " warning" : " warnings")
                      << std::endl;
        }
        for (char *warning : *item->warnings) {
            std::cout << "\033[33m    warning: " << warning << " \033[0m" << std::endl;
            std::free(warning);
        }
        item = item->next;
    }
}

bool stlInitialized = false;

bool IsSTLInitialized() {
    return stlInitialized;
}

#ifdef __BUILD_TYPE_TEST
// TODO: move main
int main(int argc, const char **argv) {
    stlInitialized = true;
    currentTest = firstTestItem;
    while (currentTest) {
        currentTest->warnings = new std::list<char *>();
        currentTest = currentTest->next;
    }
    if (allocatorStart) {
        allocatorStart();
    }
    TestingInternal::runTests();
    TestingInternal::printResults();
    if (allocatorEnd) {
        allocatorEnd();
    }
    currentTest = firstTestItem;
    while (currentTest) {
        TestItem *willBeNext = currentTest->next;
        delete currentTest->warnings;
        std::free(currentTest);
        currentTest = willBeNext;
    }
    return 0;
}

#endif