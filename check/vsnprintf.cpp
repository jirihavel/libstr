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

TEST_CASE("vsnprintf returns correct length", "[vsnprintf]")
{
    char test[20];
    for(size_t capacity = 0; capacity < 21; ++capacity)
    {
        CAPTURE(capacity)
        int length = test_fun(test, capacity, "%d 3 %s", 666, "abcfjfjFUJ");
        CHECK(length == 16);
    }
}
