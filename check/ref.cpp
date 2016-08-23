#include <str/ref.h>

#include "catch.hpp"

void check_ref_behaves_empty(StrRef ref)
{
    THEN("Ref behaves as empty")
    {
        CHECK(str_ref_is_empty(ref));
        CHECK(str_ref_len(ref) == 0);

        char const * ptr = str_ref_ptr(ref);
        CHECK(ptr);
        CHECK(*ptr == '\0');
    }
}

void check_ref_zero_term(StrRef ref)
{
    THEN("Ref is zero terminated")
    {
        CHECK(str_ref_is_zero_term(ref));
        CHECK(str_ref_is_read_term(ref));
        CHECK(str_ref_ptr(ref)[str_ref_len(ref)] == '\0');
    }
}

void check_ref_null(StrRef ref)
{
    THEN("Explicit null test is true")
        CHECK(str_ref_is_null(ref));
    check_ref_behaves_empty(ref);
    check_ref_zero_term(ref);
}

void check_ref_empty(StrRef ref)
{
    THEN("Explicit null test is false")
        CHECK(!str_ref_is_null(ref));
    check_ref_behaves_empty(ref);
}

TEST_CASE("StrRef special values", "[ref]")
{
    GIVEN("null ref")
        check_ref_null(str_ref_null());
    GIVEN("empty ref")
        check_ref_empty(str_ref_empty());  

    CHECK(str_ref_cmp_eq(str_ref_null(), str_ref_empty()));

    GIVEN("null cstr ref")
    {
        StrRef ref = str_ref_cstr(nullptr);
        check_ref_null(ref);
        CHECK(str_ref_cmp_eq(ref, str_ref_null()));
        CHECK(str_ref_cmp_eq(ref, str_ref_empty()));
    }
    GIVEN("empty cstr ref")
    {
        StrRef ref = str_ref_cstr("");
        check_ref_empty(ref);
        CHECK(str_ref_cmp_eq(ref, str_ref_null()));
        CHECK(str_ref_cmp_eq(ref, str_ref_empty()));
    }
}

TEST_CASE("StrRef find", "[ref]")
{
    GIVEN("null ref")
    {
        StrRef ref = str_ref_null();
        THEN("Character is not found")
            CHECK(str_ref_find_c(ref, 'a') == STR_LEN_MAX);
    }
    GIVEN("empty ref")
    {
        StrRef ref = str_ref_empty();
        THEN("Character is not found")
            CHECK(str_ref_find_c(ref, 'a') == STR_LEN_MAX);
    }
    GIVEN("nonempty string")
    {
        StrRef ref = str_ref_cstr("fooabcabcabc");
        GIVEN("Character in string")
            THEN("First occurence of character is found")
                CHECK(str_ref_find_c(ref, 'a') == 3);
        GIVEN("Character not in string")
            THEN("Character is not found")
                CHECK(str_ref_find_c(ref, 'z') == STR_LEN_MAX);
    }
}

TEST_CASE("StrRef tail,init", "[ref]")
{
    GIVEN("null ref")
    {
        StrRef ref = str_ref_null();
        THEN("tail 0 is null")
            CHECK(str_ref_is_null(str_ref_tail(ref, 0)));
        THEN("init 0 is null")
            CHECK(str_ref_is_null(str_ref_init(ref, 0)));
    }
    GIVEN("empty ref")
    {
        StrRef ref = str_ref_empty();
        THEN("tail 0 is empty and not null")
        {
            StrRef tail = str_ref_tail(ref, 0);
            CHECK(str_ref_is_empty(tail));
            CHECK(!str_ref_is_null(tail));
        }
        THEN("tail 1 is null")
        {
            StrRef tail = str_ref_tail(ref, 1);
            CHECK(str_ref_is_null(tail));
        }
        THEN("init 0 is empty and not null")
        {
            StrRef init = str_ref_init(ref, 0);
            CHECK(str_ref_is_empty(init));
            CHECK(!str_ref_is_null(init));
        }
        THEN("init 1 is null")
        {
            StrRef init = str_ref_init(ref, 1);
            CHECK(str_ref_is_null(init));
        }
    }
    GIVEN("single character ref")
    {
        StrRef ref = str_ref_cstr("a");
        THEN("tail 0 is not empty")
        {
            StrRef tail = str_ref_tail(ref, 0);
            CHECK(!str_ref_is_empty(tail));
        }
        THEN("tail 1 is empty and not null")
        {
            StrRef tail = str_ref_tail(ref, 1);
            CHECK(str_ref_is_empty(tail));
            CHECK(!str_ref_is_null(tail));
        }
        THEN("tail 2 is null")
        {
            StrRef tail = str_ref_tail(ref, 2);
            CHECK(str_ref_is_null(tail));
        }
        THEN("init 0 is not empty")
        {
            StrRef init = str_ref_init(ref, 0);
            CHECK(!str_ref_is_empty(init));
        }
        THEN("init 1 is empty and not null")
        {
            StrRef init = str_ref_init(ref, 1);
            CHECK(str_ref_is_empty(init));
            CHECK(!str_ref_is_null(init));
        }
        THEN("init 2 is null")
        {
            StrRef init = str_ref_init(ref, 2);
            CHECK(str_ref_is_null(init));
        }
    }
}
