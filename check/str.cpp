#include <str/str.h>

#include "catch.hpp"

void check_ref_null(StrRef ref);
void check_ref_empty(StrRef ref);

void check_str_behaves_empty(StrStr const * str)
{
    THEN("Str behaves as empty")
    {
        CHECK(str_str_is_empty(str));
        CHECK(str_str_len(str) == 0);

        char const * ptr = str_str_ptr(str);
        CHECK(ptr);
        CHECK(*ptr == '\0');
    }
}

void check_str_null(StrStr const * str)
{
    THEN("Explicit null test is true")
        CHECK(str_str_is_null(str));

    check_str_behaves_empty(str);

    WHEN("Converted to ref")
        check_ref_null(str_str_ref(str));
}

void check_str_empty(StrStr const * str)
{
    THEN("Explicit null test is false")
        CHECK(!str_str_is_null(str));

    check_str_behaves_empty(str);

    WHEN("Converted to ref")
        check_ref_empty(str_str_ref(str));
}

TEST_CASE("StrStr construction", "[str]")
{
    GIVEN("null")
    {
        StrStr str;
        str_str_init_null(&str);
    
        check_str_null(&str);

        WHEN("moved")
        {
            StrStr dst;
            str_str_init_move(&dst, &str);
            check_str_null(&dst);
            check_str_null(&str);
        }
    }

    GIVEN("null by const init")
    {
        StrStr str;
        str_str_init_const(&str, str_ref_null());

        check_str_null(&str);
    }

    GIVEN("null by weak init")
    {
        StrStr str;
        str_str_init_weak(&str, NULL, 0, 0);

        check_str_null(&str);
    }
    
    GIVEN("empty")
    {
        StrStr str;
        str_str_init_empty(&str);
    
        check_str_empty(&str);

        WHEN("moved")
        {
            StrStr dst;
            str_str_init_move(&dst, &str);
            check_str_empty(&dst);
            check_str_null(&str);
        }
    }

    GIVEN("empty by const init")
    {
        StrStr str;
        str_str_init_const(&str, str_ref_empty());

        check_str_empty(&str);
    }

    GIVEN("empty by weak init")
    {
        StrStr str;
        char buf[5] = { '\0' };
        str_str_init_weak(&str, buf, 0, 0);

        check_str_empty(&str);
    }

    GIVEN("const")
    {
        StrStr str;
        str_str_init_const(&str, str_ref_cstr("test"));

        CHECK(str_str_len(&str) == 4);
        CHECK(str_ref_cmp_eq(str_str_ref(&str), str_ref_cstr("test")));
    }
}
