/**
 * Created by Ilya Potemin on 10/5/19.
 */

#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <execinfo.h>
#include <map>
#include <new>
#include <sstream>

#ifdef __BUILD_TYPE_TEST
    #include "Testing.h"
#endif
#ifdef __BUILD_TYPE_DEBUG
    #include <cstdio>
#endif
#ifndef __BUILD_TYPE_RELEASE
    #include <cxxabi.h>
#endif

typedef size_t Size;

#define UNUSED(expr) static_cast<void>(expr)

struct ObjectHeader {
    Size size;
    char markword;
};

const auto OBJECT_HEADER_SIZE = sizeof(ObjectHeader);
const auto OBJECT_MARKWORD_OFFSET = offsetof(ObjectHeader, markword);

struct ArrayHeader {
    Size size;
    char markword;
};

const auto ARRAY_HEADER_SIZE = sizeof(ArrayHeader);
const auto ARRAY_MARKWORD_OFFSET = offsetof(ArrayHeader, markword);

static_assert(
    (OBJECT_HEADER_SIZE - OBJECT_MARKWORD_OFFSET) == (ARRAY_HEADER_SIZE - ARRAY_MARKWORD_OFFSET),
    "it will be unable to extract object/array markword by pointer due to different alignment");

const Size MARKWORD_END_OFFSET = OBJECT_HEADER_SIZE - OBJECT_MARKWORD_OFFSET;

bool recurseLock = false;

#ifdef __BUILD_TYPE_TEST

struct TestAllocation {
    char type;
    Size size;
    void *realPtr;
    char *allocatedBy;
};

std::map<void *, TestAllocation> testAllocations;

void OnTestEvent(Testing *t, TestingEvent event) {
    UNUSED(t);
    recurseLock = true;
    switch (event) {
        case TestingEvent::START_TEST: {
            testAllocations.clear();
            break;
        }
        case TestingEvent::END_TEST: {
            for (auto pair : testAllocations) {
                std::stringstream bufferStream;
                bufferStream << "Non-freed variable of type "
                             << (pair.second.type == 'o' ? "object" : "array") << " at "
                             << pair.first << " with size " << pair.second.size << " allocated by "
                             << pair.second.allocatedBy;
                const std::string &bufferStr = bufferStream.str();
                auto buffer = static_cast<char *>(std::malloc(bufferStr.size() + 1));
                std::memcpy(buffer, bufferStr.c_str(), bufferStr.size() + 1);
                Testing::reportWarning(static_cast<char *&&>(buffer));
                std::free(pair.second.realPtr);
            }
            break;
        }
    }
    recurseLock = false;
}

void AttachTestAllocationsTrace() {
    recurseLock = true;
    Testing::attachTestEventCallback(OnTestEvent);
    recurseLock = false;
}

void DetachTestAllocationsTrace() {
    recurseLock = true;
    Testing::detachTestEventCallback(OnTestEvent);
    recurseLock = false;
}

__attribute__((constructor(240))) void AttachToTests() {
    allocatorStart = AttachTestAllocationsTrace;
    allocatorEnd = DetachTestAllocationsTrace;
}

#endif

inline char *GetOperatorCallerName() {
    int callerDepth = 4;
    void *traceBuffer[callerDepth];
    int res = backtrace(static_cast<void **>(traceBuffer), callerDepth);
    callerDepth = callerDepth > res ? res : callerDepth;
    auto strings = backtrace_symbols(static_cast<void *const *>(traceBuffer), callerDepth);
    auto callerStr = strings[callerDepth - 1];

#ifdef __COMPILER_FLAG_RDYNAMIC
    // select required part of caller description
    int status = 0;
    while (callerStr[0] != '(') {
        ++callerStr;
    }
    callerStr += 1;
    int end = 0;
    while (callerStr[end] != '+') {
        ++end;
    }
    callerStr[end] = '\0';
    // demangle C++ function name
    auto retStr = abi::__cxa_demangle(callerStr, nullptr, nullptr, &status);
#else  // __COMPILER_FLAG_RDYNAMIC
    int slash = 0;
    for (int i = 0; callerStr[i] != '('; i++) {
        if (callerStr[i] == '/') {
            slash = i + 1;
        }
    }
    auto retStr = strdup(callerStr + slash);
#endif // __COMPILER_FLAG_RDYNAMIC

    free(strings);
    return retStr;
}

inline void *AssertAllocationDone(void *ptr) {
    if (ptr == nullptr) {
        throw std::bad_alloc();
    } else {
        return ptr;
    }
}

__attribute_noinline__ void *AllocateManaged(bool isArray, Size size, Size count) {
    UNUSED(count);
    auto headerSize = isArray ? ARRAY_HEADER_SIZE : OBJECT_HEADER_SIZE;
    auto headerMarkword = isArray ? 'a' : 'o';
    auto data = std::malloc(size + headerSize);
    auto byteData = static_cast<char *>(data);
    if (data == nullptr) {
        return nullptr;
    }

    if (isArray) {
        auto header = static_cast<ArrayHeader *>(data);
        header->markword = headerMarkword;
        header->size = size;
    } else {
        auto header = static_cast<ObjectHeader *>(data);
        header->markword = headerMarkword;
        header->size = size;
    }
    auto userPointer = static_cast<void *>(byteData + headerSize);

    if (recurseLock) {
        return userPointer;
    }
    recurseLock = true;
#ifdef __PRINT_HEAP_OPERATIONS
    auto callerName = GetOperatorCallerName();
    std::printf("Allocated %zu bytes for an %s at %p by %s\n", size, isArray ? "array" : "object",
                userPointer, callerName);
    std::free(callerName);
#endif // __PRINT_HEAP_OPERATIONS

#ifdef __BUILD_TYPE_TEST
    if (IsSTLInitialized()) {
        auto callerNameTest = GetOperatorCallerName();
        testAllocations[userPointer] = TestAllocation{
            .type = headerMarkword,
            .size = size,
            .realPtr = data,
            .allocatedBy = callerNameTest,
        };
    }
#endif
    recurseLock = false;
    return userPointer;
}

inline bool SoftAssertMarkword(void *userPointer, char markword) {
    auto dataByte = static_cast<char *>(userPointer);
    auto markwordPtr = dataByte - MARKWORD_END_OFFSET;
    auto realMarkword = *markwordPtr;
    return realMarkword == markword;
}

__attribute_noinline__ bool DeallocateManaged(bool isArray, void *ptr) {
    if (ptr == nullptr) {
        return false;
    }
    auto callerName = GetOperatorCallerName();
    if (!SoftAssertMarkword(ptr, isArray ? 'a' : 'o')) {
        std::stringstream bufferStream;
        bufferStream << "Wrong delete was called for pointer " << ptr << " by " << callerName
                     << " : deallocation of type " << (isArray ? "array" : "object")
                     << " requested, type " << (!isArray ? "array" : "object") << " found";
        const std::string &bufferStr = bufferStream.str();
        auto buffer = static_cast<char *>(std::malloc(bufferStr.size() + 1));
        std::memcpy(buffer, bufferStr.c_str(), bufferStr.size() + 1);
#ifdef __BUILD_TYPE_TEST
        if (IsSTLInitialized()) {
            recurseLock = true;
            Testing::reportWarning(static_cast<char *&&>(buffer));
            recurseLock = false;
        }
#endif // __BUILD_TYPE_TEST

#ifdef __BUILD_TYPE_DEBUG
        printf("%s\n", buffer);
        std::free(buffer);
#endif // __BUILD_TYPE_DEBUG

        return false;
    }
    auto headerSize = isArray ? ARRAY_HEADER_SIZE : OBJECT_HEADER_SIZE;
    auto byteData = static_cast<char *>(ptr);
    auto headerRaw = static_cast<void *>(byteData - headerSize);

    if (recurseLock) {
        return true;
    }
    recurseLock = true;

#ifdef __PRINT_HEAP_OPERATIONS
    int size = 0;
    if (isArray) {
        auto header = static_cast<ArrayHeader *>(headerRaw);
        size = header->size;
    } else {
        auto header = static_cast<ObjectHeader *>(headerRaw);
        size = header->size;
    }
    std::printf("Deallocated %d bytes from an %s at %p by %s\n", size, isArray ? "array" : "object",
                ptr, callerName);
#endif // __PRINT_HEAP_OPERATIONS

#ifdef __BUILD_TYPE_TEST
    if (IsSTLInitialized()) {
        // std::free(testAllocations[ptr].allocatedBy);
        testAllocations.erase(ptr);
    }
#endif

    recurseLock = false;

    std::free(headerRaw);
    return true;
}

void *operator new(Size size, std::align_val_t al, const std::nothrow_t &tag) noexcept {
    UNUSED(al);
    UNUSED(tag);
    return AllocateManaged(false, size, 0);
}

void *operator new[](Size size, std::align_val_t al, const std::nothrow_t &tag) noexcept {
    UNUSED(al);
    UNUSED(tag);
    return AllocateManaged(true, size, 0);
}

void *operator new(Size size, std::align_val_t al) {
    UNUSED(al);
    void *ptr = AllocateManaged(false, size, 0);
    return AssertAllocationDone(ptr);
}

void *operator new[](Size size, std::align_val_t al) {
    UNUSED(al);
    void *ptr = AllocateManaged(true, size, 0);
    return AssertAllocationDone(ptr);
}

void *operator new(Size size, const std::nothrow_t &tag) noexcept {
    UNUSED(tag);
    return AllocateManaged(false, size, 0);
}

void *operator new[](Size size, const std::nothrow_t &tag) noexcept {
    UNUSED(tag);
    return AllocateManaged(true, size, 0);
}

void *operator new(Size size) {
    void *ptr = AllocateManaged(false, size, 0);
    return AssertAllocationDone(ptr);
}

void *operator new[](Size size) {
    void *ptr = AllocateManaged(true, size, 0);
    return AssertAllocationDone(ptr);
}

void operator delete(void *ptr) noexcept {
    DeallocateManaged(false, ptr);
}

void operator delete(void *ptr, std::size_t size) noexcept {
    UNUSED(size);
    DeallocateManaged(false, ptr);
}

void operator delete(void *ptr, std::align_val_t al) noexcept {
    UNUSED(al);
    DeallocateManaged(false, ptr);
}

void operator delete(void *ptr, std::size_t size, std::align_val_t al) noexcept {
    UNUSED(size);
    UNUSED(al);
    DeallocateManaged(false, ptr);
}

void operator delete(void *ptr, std::nothrow_t const &flag) noexcept {
    UNUSED(flag);
    DeallocateManaged(false, ptr);
}

void operator delete(void *ptr, std::align_val_t al, std::nothrow_t const &flag) noexcept {
    UNUSED(al);
    UNUSED(flag);
    DeallocateManaged(false, ptr);
}

void operator delete[](void *ptr) noexcept {
    DeallocateManaged(true, ptr);
}

void operator delete[](void *ptr, std::size_t size) noexcept {
    UNUSED(size);
    DeallocateManaged(true, ptr);
}

void operator delete[](void *ptr, std::align_val_t al) noexcept {
    UNUSED(al);
    DeallocateManaged(true, ptr);
}

void operator delete[](void *ptr, std::size_t size, std::align_val_t al) noexcept {
    UNUSED(size);
    UNUSED(al);
    DeallocateManaged(true, ptr);
}

void operator delete[](void *ptr, std::nothrow_t const &flag) noexcept {
    UNUSED(flag);
    DeallocateManaged(true, ptr);
}

void operator delete[](void *ptr, std::align_val_t al, std::nothrow_t const &flag) noexcept {
    UNUSED(al);
    UNUSED(flag);
    DeallocateManaged(true, ptr);
}
