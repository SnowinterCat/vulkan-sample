#include <print>

consteval auto operator""_tr(const char *str, [[maybe_unused]] uint64_t unused)
{
    return str;
}

int main()
{
    std::println("hello" // aaaaa
                 "(\"111"
                 " world"_tr);
    std::println("你好" // aaaaa
                 " 世界");
    std::println("你好 c++ {}"_tr, "aaa", "bbb"_tr);
    std::println("这行不翻译");
    return 0;
}