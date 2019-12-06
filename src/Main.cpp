/**
 * @author Ilya Potemin <i.potemin@innopolis.ru>
 * @author Kamilla Kryachkina <k.kryachkina@innopolis.ru>
 * @author Kamilya Timchenko <k.timchenko@innopolis.ru>
 * @author Evgeniy Bondarev <e.bondarev@innopolis.ru>
 */

#include <iostream>
#include <map>
#ifdef _WIN64
    #include <io.h>
    #define PLATFORM_getcwd _getcwd
#else
    #ifdef __linux__
        #include <zconf.h>
    #else
        #include <zconf.h>
    #endif
    #define PLATFORM_getcwd getcwd
#endif
#include "Codegen.h"
#include "Config.h"
#include "Lexer.h"
#include "Semantics.h"
#include "Utils.h"

std::map<std::string, OptionHandle> longOptionMap;
std::map<std::string, OptionHandle> shortOptionMap;

long long GetAndIncrementUniqueCounter(Counter c) {
    static long long counter[] = {0, 0, 0};
    return counter[c]++;
}

int RegisterOption(const char *longOption, const char *shortOption, int argsLength,
                   OptionHandlerFunc handler, const char *description) {
    auto optionHandle = OptionHandle{
        .argsLength = argsLength,
        .handlerFunc = handler,
        .shortOption = shortOption,
        .longOption = longOption,
        .description = description,
    };
    longOptionMap["--" + std::string(longOption)] = optionHandle;
    shortOptionMap["-" + std::string(shortOption)] = optionHandle;
    return 0;
}

int ReadOptions(int argc, const char *argv[]) {
    for (int i = 1; i < argc; i++) {
        auto argStr = std::string(argv[i]);
        if (argStr.rfind("--") == 0 || argStr.rfind("-") == 0) {
            // long or short option
            OptionHandle handle;
            if (longOptionMap.count(argStr) == 1) {
                handle = longOptionMap[argStr];
            } else if (shortOptionMap.count(argStr) == 1) {
                handle = shortOptionMap[argStr];
            } else {
                std::cout << "There is no such option : " << argStr << std::endl;
                return 1;
            }
            if (argc - i < handle.argsLength) {
                std::cout << "Option " << argStr << " requires " << handle.argsLength
                          << "arguments, " << argc - i << " arguments left" << std::endl;
            }
            auto retValue = handle.handlerFunc(argv + i + 1);
            if (retValue == 0) {
                i += handle.argsLength;
            } else {
                std::cout << "Unable to process option " << argStr << ", exiting";
                return 2;
            }
        }
    }
    return 0;
}

int HandleOptionInput(const char *options[]) {
    auto inputSource = std::string(options[0]);
    if (inputSource == "stdin") {
        SetLexerInput(stdin);
    } else {
        FILE *file = fopen(inputSource.c_str(), "rb");
        if (file == nullptr) {
            std::cout << "Unable to open source file " << inputSource << std::endl;
            return 1;
        }
        std::cout << " << Processing file " << inputSource << " >> " << std::endl;
        SetLexerInput(file);
    }
    return 0;
}

FILE *outputFile = stdout;

int HandleOptionOutput(const char *options[]) {
    auto output = std::string(options[0]);
    if (output == "stdout") {
        outputFile = stdout;
    } else {
        FILE *file = fopen(output.c_str(), "wb");
        if (file == nullptr) {
            std::cout << "Unable to open output file " << output << std::endl;
            return 1;
        }
        outputFile = file;
    }
    return 0;
}

int HandleOptionHelp(const char *options[]) {
    std::cout << "Options available: " << std::endl;
    for (std::pair<std::string, OptionHandle> pair : shortOptionMap) {
        OptionHandle &handle = pair.second;
        std::cout << "  --" << handle.longOption << " / -" << handle.shortOption << " - "
                  << handle.description << std::endl;
    }
    std::exit(0);
}

#ifndef __BUILD_TYPE_RELEASE
bool componentTraceMap[] = {false, false, false, false, false, false};
const char *componentNameMap[] = {
    "\033[32m[env      ]\033[0m", "\033[32m[lexer    ]\033[0m", "\033[32m[parser   ]\033[0m",
    "\033[34m[scope    ]\033[0m", "\033[33m[semantics]\033[0m", "\033[35m[codegen  ]\033[0m",
};

bool IsComponentTraced(Component component) {
    return componentTraceMap[component];
}

std::ostream &GetTraceStream(Component component) {
    return std::cout << componentNameMap[component] << ": ";
}

int HandleOptionTrace(const char *options[]) {
    auto componentStr = std::string(options[0]);
    if (componentStr == "env") {
        componentTraceMap[COMPONENT_ENV] = true;
        std::cout << "ENV tracing is enabled" << std::endl;
        const int maxFilePathLength = 256;
        char buffer[maxFilePathLength];
        char *bufferPtr = static_cast<char *>(buffer);
        PLATFORM_getcwd(bufferPtr, maxFilePathLength);
        GetTraceStream(COMPONENT_ENV)
            << "[env] Current workdir : " << std::string(bufferPtr) << std::endl;
    }
    if (componentStr == "lexer") {
        componentTraceMap[COMPONENT_LEXER] = true;
        std::cout << "LEXER tracing is enabled" << std::endl;
    }
    if (componentStr == "parser") {
        std::cout << "PARSER tracing is enabled" << std::endl;
        componentTraceMap[COMPONENT_PARSER] = true;
        yydebug = 1;
    }
    if (componentStr == "scope") {
        componentTraceMap[COMPONENT_SCOPE] = true;
        std::cout << "SCOPE tracing is enabled" << std::endl;
    }
    if (componentStr == "semantics") {
        componentTraceMap[COMPONENT_SEMANTICS] = true;
        std::cout << "SEMANTICS tracing is enabled" << std::endl;
    }
    if (componentStr == "codegen") {
        componentTraceMap[COMPONENT_CODEGEN] = true;
        std::cout << "CODEGEN tracing is enabled" << std::endl;
    }
    return 0;
}
#else
int HandleOptionTrace(const char *options[]) {
    std::cerr << "Any trace flags are ignored while compiled in RELEASE type" << std::endl;
    return 0;
}
#endif // RELEASE TYPE

int StartILangCompiler(int argc, const char *argv[]) {
    int status = 0;
#ifdef __BUILD_TYPE_DEBUG
    yydebug = 0;
#endif

    RegisterOption("input", "s", 1, HandleOptionInput,
                   "set to \"stdin\" to read source from the default input, or name a source file");
    RegisterOption("output", "o", 1, HandleOptionOutput,
                   "set to \"stdout\" to write compilation result to the default output");
    RegisterOption("trace", "t", 1, HandleOptionTrace,
                   "enables verbose output for required components");
    RegisterOption("help", "h", 0, HandleOptionHelp,
                   "program usage help, shows this message and exits");

    if ((status = ReadOptions(argc, argv)) != 0) {
        std::exit(status);
    }
    if (GetLexerInput() == nullptr) {
        SetLexerInput(stdin);
    }

    Semantics::InitSemanticContext();
    // cleanup
    Codegen::InitCodegenContext("compilation_result", outputFile);
    // parsing
    yyparse();
    // semantic analysis
    Semantics::RunSemanticPass(GetProgramTree());
    // codegeneration
    Codegen::RunCodegenPass(GetProgramTree());
    // cleanup
    Codegen::DestroyCodegenContext();
    Semantics::DestroySemanticContext();
    longOptionMap.clear();
    shortOptionMap.clear();
    return 0;
}

#ifndef __BUILD_TYPE_TEST
int main(int argc, const char *argv[]) {
    return StartILangCompiler(argc, argv);
}
#endif