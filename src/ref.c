#include <str/ref.h>

// -- Constructors --

StrRef str_ref(char const * ptr, size_t len);
StrRef str_ref_null();
StrRef str_ref_empty();
StrRef str_ref_cstr(char const * str);

// -- Queries --

bool str_ref_is_null(StrRef ref);
bool str_ref_is_empty(StrRef ref);

// -- Access --

size_t str_ref_len(StrRef ref);

/** \brief Get pointer.
 *
 * Returned pointer is never NULL even for null string.
 */
char const * str_ref_ptr(StrRef ref);

// -- Substrings --

/** \brief Returns string tail without first n characters.
 *
 * n==0 is identity.
 * If n > string length, null string is returned.
 * null -> null
 */
StrRef str_ref_tail(StrRef ref, size_t n);

/** \brief Returns n initial characters.
 *
 * If n >= string length, original string is returned.
 * null -> null
 */
StrRef str_ref_init(StrRef ref, size_t n);

/** \brief Returns substring of len characters, starting at idx.
 *
 * if len is too big, it is shortened, idx==0 is identity.
 * if idx is too big, null is returned.
 */
StrRef str_ref_substr(StrRef ref, size_t idx, size_t len);

// -- Decomposition --

/** \brief Find position of character in string.
 *
 * Returns SIZE_MAX if not found.
 */
size_t str_ref_find_c(StrRef ref, char c)
{
    STR_REF_ASSERT(&ref);
    char const * const ptr = memchr(ref.ptr, c, ref.len);
    assert(!ptr || (ptr >= ref.ptr));
    return ptr ? (size_t)(ptr - ref.ptr) : SIZE_MAX;
}

/** \brief Cut and return first word.
 *
 * Finds first occurence of character c.
 * Returns initial part of the string up to c,
 *  removes returned part and the separator from ref.
 *
 * StrRef tmp = in;
 * while(!str_ref_is_empty(tmp))
 * {
 *   StrRef word = str_ref_word_c(&tmp, ' ');
 *   foo(word);
 * }
 */
StrRef str_ref_word_c(StrRef * ref, char c)
{
    STR_REF_ASSERT(ref);
    char const * const ptr = memchr(ref->ptr, c, ref->len);
    StrRef ret;
    ret.ptr = ref->ptr;
    if(ptr)
    {
        ret.len = ptr - ref->ptr;
        assert(ret.len < ref->len);
        ref->ptr += ret.len + 1;
        ref->len -= ret.len + 1;
    }
    else
    {
        ret.len = ref->len;
        ref->ptr += ref->len;
        ref->len = 0;
    }
    return ret;
}

/** \brief Chop trailing spaces.
 */
StrRef str_ref_chop_spaces(StrRef ref);

/** \brief Trim leading and trailing spaces.
 */
StrRef str_ref_trim_spaces(StrRef ref);
