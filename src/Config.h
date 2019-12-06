/**
 * @author Ilya Potemin <i.potemin@innopolis.ru>
 * @author Kamilla Kryachkina <k.kryachkina@innopolis.ru>
 * @author Kamilya Timchenko <k.timchenko@innopolis.ru>
 * @author Evgeniy Bondarev <e.bondarev@innopolis.ru>
 */

#ifndef CC_PROJECT_MAIN_H
#define CC_PROJECT_MAIN_H

#include <iostream>
#include <ostream>

typedef int (*OptionHandlerFunc)(const char *options[]);

struct OptionHandle {
    int argsLength;
    OptionHandlerFunc handlerFunc;
    const char *shortOption;
    const char *longOption;
    const char *description;
};

int StartILangCompiler(int argc, const char *argv[]);

int RegisterOption(const char *longOption, const char *shortOption, int argsLength,
                   OptionHandlerFunc handler, const char *description);

enum Component {
    COMPONENT_ENV = 0,
    COMPONENT_LEXER = 1,
    COMPONENT_PARSER = 2,
    COMPONENT_SCOPE = 3,
    COMPONENT_SEMANTICS = 4,
    COMPONENT_CODEGEN = 5,
};

#ifndef __BUILD_TYPE_RELEASE
bool IsComponentTraced(Component component);

std::ostream &GetTraceStream(Component component);
#else
inline bool IsComponentTraced(Component component) {
    return false;
}

inline std::ostream &GetTraceStream(Component component) {
    return std::cerr;
}
#endif

#endif // CC_PROJECT_MAIN_H
