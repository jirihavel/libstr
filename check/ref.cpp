#include <str/ref.h>

#include "catch.hpp"

void check_ref_behaves_empty(StrRef const & ref)
{
    CHECK(str_ref_is_empty(ref));
    CHECK(str_ref_len(ref) == 0);

    char const * ptr = str_ref_ptr(ref);
    CHECK(ptr);
    CHECK(*ptr == '\0');
}

void check_ref_null(StrRef const & ref)
{
    CHECK(str_ref_is_null(ref));
    
    check_ref_behaves_empty(ref);
}

void check_ref_empty(StrRef const & ref)
{
    CHECK(!str_ref_is_null(ref));

    check_ref_behaves_empty(ref);
}

TEST_CASE("null ref", "[ref]")
{
    StrRef ref = str_ref_null();

    check_ref_null(ref);
}

TEST_CASE("empty ref", "[ref]")
{
    StrRef ref = str_ref_empty();

    check_ref_empty(ref);  
}
