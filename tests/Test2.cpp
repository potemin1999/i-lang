/**
 * Created by Ilya Potemin on 11/10/19.
 */

#include "Config.h"
#include "testing/Testing.h"

UNIT_TEST(Test2_1) {
    const auto argumentCount = 13;
    const char *test1Flags[] = {"/compiler_exe", "--input", "tests/res/test2_1.i",
                                //"--trace","lexer",
                                //"--trace","parser",
                                "--trace", "scope", "--trace", "semantics", "--trace", "codegen",
                                "--trace", "env", "--output", "test2_1.bc"};
    auto result = StartILangCompiler(argumentCount, static_cast<const char **>(test1Flags));
    if (result != 0) {
        Testing::fail();
    }
    if (std::system("llc-9 test2_1.bc") != 0) {
        Testing::fail();
    }
    std::system("lli-9 test2_1.bc");
}