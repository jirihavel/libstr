#include <str/fmt.h>

#include "catch.hpp"

TEST_CASE("str_add_char", "[fmt]")
{
    char tmp[2];

    GIVEN("Too small output array")
        for(size_t c = 0; c < sizeof(tmp); ++c)
        {
            char * dst = tmp;
            size_t cap = c;
            size_t len = str_add_char(&dst, &cap, 'a');
            THEN("str_add_char resets the output")
            {
                CHECK( !dst );
                CHECK( cap == 0 );
            }
            THEN("str_add_char returns expected length")
                CHECK( len == 1 );
        }

    GIVEN("Sufficient array")
    {
        char * dst = tmp;
        size_t cap = sizeof(tmp);
        size_t len = str_add_char(&dst, &cap, 'a');
        THEN("Output is zero terminated")
            CHECK( *dst == '\0' );
        THEN("Output is correct")
            CHECK( strcmp(tmp, "a") == 0 );
        THEN("dst and cap advance correctly")
        {
            CHECK( cap == 1 );
            CHECK( len == 1 );
            CHECK( dst == tmp + len );
        }
    }
}
/*
TEST_CASE("str_enc_www_form_component", "[fmt]")
{
    char tmp[256];

    char * dst;
    size_t cap, len;

    for(size_t c = 0; c < 4; ++c)
    {
        tmp[0] = 'a';
        dst = tmp;
        cap = c;
        len = str_enc_www_form_component(&dst, &cap, str_ref("abc", 3));
        CHECK( !dst );
        CHECK( cap == 0 );
        CHECK( len == 3 );
        if(c > 0)
            CHECK(tmp[0] == '\0');
    }
    dst = tmp;
    cap = 4;
    len = str_enc_www_form_component(&dst, &cap, str_ref("abc", 3));
    CHECK( *dst == '\0' );
    CHECK( cap == 1 );
    CHECK( len == 3 );
    CHECK( dst == tmp + len );
    CHECK( strcmp(tmp, "abc") == 0 );
    
    std::string sa{"Text with  some spaces"}, da{"Text+with++some+spaces"};

    dst = tmp;
    cap = sizeof(tmp);
    len = str_enc_www_form_component(&dst, &cap, str_ref(sa.data(), sa.size()));
    CHECK( *dst == '\0');
    CHECK( dst == tmp + len);
    CHECK( da == tmp );
    
    std::string sb{"foo @+%/"}, db{"foo+%40%2B%25%2F"};

    dst = tmp;
    cap = sizeof(tmp);
    len = str_enc_www_form_component(&dst, &cap, str_ref(sb.data(), sb.size()));
    CHECK( *dst == '\0');
    CHECK( dst == tmp + len);
    CHECK( db == tmp );

    std::string sc{"sales and marketing/Miami"}, dc{"sales+and+marketing%2FMiami"};

    dst = tmp;
    cap = sizeof(tmp);
    len = str_enc_www_form_component(&dst, &cap, str_ref(sc.data(), sc.size()));
    CHECK( *dst == '\0');
    CHECK( dst == tmp + len);
    CHECK( dc == tmp );
}
*/
/*
BOOST_AUTO_TEST_CASE( str_simple_test )
{
    StrStrong str;
    strs_init(&str);

    BOOST_CHECK( !str.str );
    BOOST_CHECK_EQUAL( str.cap, 0 );
    BOOST_CHECK_EQUAL( str.len, 0 );
    
    BOOST_CHECK( strs_alloc(&str, 0, 666) );
    BOOST_CHECK( str.str );
    BOOST_CHECK_EQUAL( str.str[0], '\0' );
    BOOST_CHECK_EQUAL( str.cap, 0 );
    BOOST_CHECK_EQUAL( str.len, 0 );

    BOOST_CHECK( strs_alloc(&str, 666, -1) );
    BOOST_CHECK( str.str );
    BOOST_CHECK_EQUAL( str.cap, 666 );
    BOOST_CHECK_EQUAL( str.len, -1 );

    for(int i = 0; i <= 666; ++i)
        str.str[i] = i;

    strs_kill(&str);
}

BOOST_AUTO_TEST_CASE( str_printf_test )
{
    StrStrong str;
    strs_init(&str);

    BOOST_CHECK( !str.str );
    BOOST_CHECK_EQUAL( str.cap, 0 );
    BOOST_CHECK_EQUAL( str.len, 0 );
    
    BOOST_CHECK( strs_printf(&str, "%d abc %d %s", 123, 456, "def") );
    BOOST_CHECK_EQUAL( str.len, strlen(str.str) );
    BOOST_CHECK_EQUAL( str.str, "123 abc 456 def" );

    size_t const old_cap = str.cap;

    BOOST_CHECK( strs_printf(&str, "%d abc", 123) );
    BOOST_CHECK_EQUAL( str.cap, old_cap );
    BOOST_CHECK_EQUAL( str.len, strlen(str.str) );
    BOOST_CHECK_EQUAL( str.str, "123 abc" );

    strs_kill(&str);
}
*/
