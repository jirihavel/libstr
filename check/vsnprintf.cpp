#include <str/vsnprintf.h>

#include "catch.hpp"

int test_fun(char * dst, size_t cap, char const * fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    int const len = str_vsnprintf(dst, cap, fmt, args);
    va_end(args);
    return len;
}

TEST_CASE("vsnprintf", "[vsnprintf]")
{
    char test[20];
    for(size_t i = 0; i < 21; ++i)
    {
        int c = test_fun(test, i, "%d 3 %s", 666, "abcfjfjFUJ");
        CHECK(c == 16);
    }
}
