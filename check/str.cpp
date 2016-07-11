#include <str/str.h>

#include "catch.hpp"

void check_ref_null(StrRef const & ref);
void check_ref_empty(StrRef const & ref);

void check_str_null(StrStr const * str)
{
    CHECK(str_str_is_null(str));

    StrRef ref = str_str_ref(str);
    check_ref_null(ref);
}

void check_str_empty(StrStr const * str)
{
    CHECK(!str_str_is_null(str));

    StrRef ref = str_str_ref(str);
    check_ref_empty(ref);
}

TEST_CASE("null str", "[str]")
{
    StrStr str;
    str_str_init_null(&str);

    check_str_null(&str);

    str_str_init_weak(&str, NULL, 0, 0);
    check_str_null(&str);
}

TEST_CASE("empty str", "[str]")
{
    StrStr str;
    str_str_init_empty(&str);
    
    check_str_empty(&str);

    char buf[5] = { '\0' };
    str_str_init_weak(&str, buf, -1, 5);
    check_str_empty(&str);

    StrStr aux;
    str_str_init_move(&aux, &str);
    check_str_empty(&aux);
    check_str_null(&str);
}
