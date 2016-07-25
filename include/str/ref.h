#ifndef LIBSTR_REF_H_INCLUDED
#define LIBSTR_REF_H_INCLUDED

#include <str/api.h>

#ifdef __cplusplus
extern "C" {
#endif

// -- Interface --

#include <limits.h>
#include <stdbool.h>
#include <stddef.h>

struct StrRef_s;
typedef struct StrRef_s StrRef;

// -- Construction --

inline StrRef str_ref(char const * ptr, size_t len);
inline StrRef str_ref_null();
inline StrRef str_ref_empty();
inline StrRef str_ref_cstr(char const * str);

// -- Queries --

inline bool str_ref_is_null(StrRef ref);
inline bool str_ref_is_empty(StrRef ref);

inline bool str_ref_cmp_eq(StrRef a, StrRef b);

// -- Access --

inline size_t str_ref_len(StrRef ref);
inline char const * str_ref_ptr(StrRef ref);

// -- Substrings --

inline StrRef str_ref_tail(StrRef ref, size_t n);
inline StrRef str_ref_init(StrRef ref, size_t n);
inline StrRef str_ref_substr(StrRef ref, size_t idx, size_t len STR_DEFAULT(SIZE_MAX));

// -- Decomposition --

size_t str_ref_find_c(StrRef ref, char c);
StrRef str_ref_word_c(StrRef * ref, char c)
    __attribute__((nonnull));

inline StrRef str_ref_chop_spaces(StrRef ref);
inline StrRef str_ref_trim_spaces(StrRef ref);

// -- Implementation --

#include <assert.h>
#include <ctype.h>
#include <string.h>

/** \brief Weak reference to character array.
 *
 * It is safely copyable so it can be passed by value.
 *
 * !ptr -> null string (len == 0, special case of empty string)
 * - null strings should behave exactly as empty
 *
 * ptr : ptr[0..len-1] is readable, contents may contain '\0'
 * - readability of ptr[len] is not guaranteed
 * - don't expect the string being zero terminated
 */
struct StrRef_s
{
    char const * ptr;
    size_t len;
};

#define STR_REF_ASSERT(p) do { assert((p)); \
    assert((p)->ptr || ((p)->len == 0)); } while(false)

// -- Constructors --

inline StrRef str_ref(char const * ptr, size_t len)
{
    assert(ptr || (len == 0));
    return (StrRef) { .ptr = ptr, .len = len };
}

inline StrRef str_ref_null()
{
    return str_ref(NULL, 0);
}

inline StrRef str_ref_empty()
{
    return str_ref("", 0);
}

inline StrRef str_ref_cstr(char const * str)
{
    return str_ref(str, str ? strlen(str) : 0);
}

// -- Queries --

inline bool str_ref_is_null(StrRef ref)
{
    STR_REF_ASSERT(&ref);
    return !ref.ptr;
}

inline bool str_ref_is_empty(StrRef ref)
{
    STR_REF_ASSERT(&ref);
    return ref.len == 0;
}

inline bool str_ref_cmp_eq(StrRef a, StrRef b)
{
    STR_REF_ASSERT(&a);
    STR_REF_ASSERT(&b);
    return (a.len == b.len) && ((a.ptr == b.ptr) || (memcmp(a.ptr, b.ptr, a.len) == 0));
}

// -- Access --

inline size_t str_ref_len(StrRef ref)
{
    STR_REF_ASSERT(&ref);
    return ref.len;
}

inline char const * str_ref_ptr(StrRef ref)
{
    STR_REF_ASSERT(&ref);
    return ref.ptr ? ref.ptr : "";
}

// -- Substrings --

inline StrRef str_ref_tail(StrRef ref, size_t n)
{
    STR_REF_ASSERT(&ref);
    return n <= ref.len
        ? str_ref(ref.ptr + n, ref.len - n)
        : str_ref_null();
}

inline StrRef str_ref_init(StrRef ref, size_t n)
{
    STR_REF_ASSERT(&ref);
    return n <= ref.len
        ? str_ref(ref.ptr, n)
        : ref;
}

inline StrRef str_ref_substr(StrRef ref, size_t idx, size_t len)
{
    STR_REF_ASSERT(&ref);
    return idx <= ref.len
        ? str_ref(ref.ptr + idx, len > (ref.len-idx) ? ref.len-idx : len)
        : str_ref_null();
}

// -- Decomposition --

inline StrRef str_ref_chop_spaces(StrRef ref)
{
    STR_REF_ASSERT(&ref);
    while((ref.len > 0) && isspace(ref.ptr[ref.len-1]))
    {
        --ref.len;
    }
    return ref;
} 

inline StrRef str_ref_trim_spaces(StrRef ref)
{
    STR_REF_ASSERT(&ref);
    while((ref.len > 0) && isspace(ref.ptr[0]))
    {
        ++ref.ptr;
        --ref.len;
    }
    return str_ref_chop_spaces(ref);
} 

#ifdef __cplusplus
}
#endif

#endif//LIBSTR_REF_H_INCLUDED
