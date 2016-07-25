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

void check_ref_null(StrRef ref)
{
    THEN("Explicit null test is true")
        CHECK(str_ref_is_null(ref));
    check_ref_behaves_empty(ref);
}

void check_ref_empty(StrRef ref)
{
    THEN("Explicit null test is false")
        CHECK(!str_ref_is_null(ref));
    check_ref_behaves_empty(ref);
}

TEST_CASE("StrRef construction", "[ref]")
{
    GIVEN("null ref")
        check_ref_null(str_ref_null());
    GIVEN("empty ref")
        check_ref_empty(str_ref_empty());  
}
