#include <vks/config.hpp>
// Standard Library
#include <clocale>
#include <print>

#include "argvcvt.hpp"

int main(int argc, const char *const *argvloc)
{
    auto [argu8, argvec, argstr] = argvloc2utf8(argc, argvloc);
    std::setlocale(LC_ALL, ".UTF-8");

    for (int i = 0; i < argc; ++i) {
        std::println("{}", argu8[i]);
        for (int j = 0; argu8[i][j] != '\0'; ++j) {
            std::print("{:X} ", argu8[i][j]);
        }
        std::println();
    }
    return 0;
}