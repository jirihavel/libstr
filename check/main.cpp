#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <climits>

TEST_CASE("assumptions", "")
{
    //INT_MAX = 011111...
    CHECK((INT_MAX&(INT_MAX+1u)) == 0);
    CHECK((UINT_MAX>>1) == INT_MAX);

    // little endian
    int x = 1;
    CHECK(*((char*)&x) == 1);
}


