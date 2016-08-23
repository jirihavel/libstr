#include <str/ref.h>

// -- Constructors --

StrRef str_ref_null();
StrRef str_ref_empty();
StrRef str_ref_cstr(char const * str);
StrRef str_ref(char const * ptr, StrLen len, StrTerm term);

// -- Queries --

bool str_ref_is_null(StrRef ref);
bool str_ref_is_empty(StrRef ref);
bool str_ref_is_read_term(StrRef ref);
bool str_ref_is_zero_term(StrRef ref);

bool str_ref_cmp_eq(StrRef a, StrRef b);

// -- Access --

/** \brief Get string length.
 *
 * \return string length in [0, STR_LEN_MAX]
 */
StrLen str_ref_len(StrRef ref);

/** \brief Get pointer.
 *
 * \return pointer that is never NULL even for null string.
 */
char const * str_ref_ptr(StrRef ref);

/** \brief Convert string to memory reference.
 */
MemRef str_ref_mem(StrRef ref);

// -- Substrings --

/** \brief Returns string tail without first n characters.
 *
 * n==0 is identity.
 * If n > string length, null string is returned.
 */
StrRef str_ref_tail(StrRef ref, StrLen n);

/** \brief Return string without last n characters.
 *
 * n == 0 is identity
 * If n > string length, null string is returned.
 */
StrRef str_ref_init(StrRef ref, StrLen n);

/** \brief Returns substring of len characters, starting at idx.
 *
 * If len is too big, it is shortened.
 * If idx > string length, null is returned.
 */
StrRef str_ref_substr(StrRef ref, StrLen idx, StrLen len);

// -- Decomposition --

/** \brief Find position of character in string.
 *
 * Returns STR_LEN_MAX if not found.
 */
StrLen str_ref_find_c(StrRef ref, char c);

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
    assert(ref);
    STR_REF_ASSERT(*ref);
    StrLen const ref_len = STR_REF_LEN(*ref);
    char const * const ptr = memchr(ref->ptr, c, ref_len);
    if(ptr)
    {
        assert(ptr >= ref->ptr);
        StrLen const len = ptr - ref->ptr;
        *ref = STR_REF_TAIL(*ref, len + 1);
        return STR_REF(ptr, len, true);
    }
    else
    {
        StrRef ret = STR_REF(ref->ptr, ref_len, ref->flg & 0x1);
        *ref = STR_REF(ref->ptr + ref_len, 0, ref->flg & 0x1);
        return ret;
    }
}

static inline StrRef STR_REF_CHOP(StrRef ref, int (*is_char)(int))
{
    unsigned len = STR_REF_LEN(ref);
    bool read_term = ref.flg & 0x1;
    while((len > 0) && is_char(ref.ptr[len]))
    {
        read_term = true;
        --len;
    }
    ref.flg = (len << 1) + read_term;
    return ref;
} 

StrRef str_ref_chop(StrRef ref, int (*is_char)(int))
{
    STR_REF_ASSERT(ref);
    return STR_REF_CHOP(ref, is_char);
}

StrRef str_ref_trim(StrRef ref, int (*is_char)(int))
{
    STR_REF_ASSERT(ref);
    unsigned len = STR_REF_LEN(ref);
    while((len > 0) && is_char(ref.ptr[0]))
    {
        ++ref.ptr;
        --len;
    }
    ref.flg = (len << 1) + (ref.flg & 0x1);
    return STR_REF_CHOP_SPACES(ref);
}

/** \brief Chop trailing spaces.
 */
StrRef str_ref_chop_spaces(StrRef ref);

/** \brief Trim leading and trailing spaces.
 */
StrRef str_ref_trim_spaces(StrRef ref);
