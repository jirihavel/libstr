#ifndef LIBSTR_REF_H_INCLUDED
#define LIBSTR_REF_H_INCLUDED

#include <str/api.h>
#include <str/mem.h>

#ifdef __cplusplus
extern "C" {
#endif

// -- Interface --

#include <assert.h>
#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

struct StrRef_s;
typedef struct StrRef_s StrRef;

// -- Construction --

inline StrRef str_ref_null();
inline StrRef str_ref_empty();
inline StrRef str_ref_cstr(char const * str);
inline StrRef str_ref(char const * ptr, StrLen len, StrTerm term STR_DEFAULT(STR_TERM_NONE));

// -- Queries --

inline bool str_ref_is_null(StrRef ref);
inline bool str_ref_is_empty(StrRef ref);
inline bool str_ref_is_read_term(StrRef ref);
inline bool str_ref_is_zero_term(StrRef ref);

inline bool str_ref_cmp_eq(StrRef a, StrRef b);

// -- Access --

inline char const * str_ref_ptr(StrRef ref);
inline StrLen str_ref_len(StrRef ref);
inline MemRef str_ref_mem(StrRef ref);

// -- Substrings --

inline StrRef str_ref_tail(StrRef ref, StrLen n STR_DEFAULT(1));
inline StrRef str_ref_init(StrRef ref, StrLen n STR_DEFAULT(1));
inline StrRef str_ref_substr(StrRef ref, StrLen idx, StrLen len STR_DEFAULT(STR_LEN_MAX));

// -- Decomposition --

inline StrLen str_ref_find_c(StrRef ref, char c);
StrRef str_ref_word_c(StrRef * ref, char c)
    __attribute__((nonnull));

StrRef str_ref_chop(StrRef ref, int (*is_char)(int));
StrRef str_ref_trim(StrRef ref, int (*is_char)(int));

inline StrRef str_ref_chop_spaces(StrRef ref);
inline StrRef str_ref_trim_spaces(StrRef ref);

// -- Implementation --

/** \brief Weak reference to string.
 *
 * It is safely copyable so it can be passed by value.
 *
 * !ptr -> null string (len == 0, special case of empty string)
 * - null strings should behave exactly as empty
 * - flg == 1 - str_ref_ptr returns "", that is read_term
 *
 * ptr : ptr[0..len-1] is readable, contents mustn't contain '\0'
 * - readability of ptr[len] is not guaranteed
 * - don't expect the string being zero terminated
 *
 * read_term : ptr[len] is readable
 * - still may not be zero terminated, but it can be checked
 */
struct StrRef_s
{
    char const * ptr;
    unsigned flg; // (len << 1) | read_term
};

/** \brief Check all invariants.
 */
#define STR_REF_ASSERT(ref) do {                        \
    assert((ref).ptr || ((ref).flg == 1));              \
    assert(!memchr((ref).ptr, '\0', STR_REF_LEN(ref))); \
    } while(false)

// -- Constructors --

inline StrRef STR_REF(char const * ptr, StrLen len, bool read_term)
{
    return (StrRef) { .ptr = ptr, .flg = ((unsigned)len<<1) | (read_term || !ptr) };
}

inline StrRef str_ref(char const * ptr, StrLen len, StrTerm term)
{
    assert(len >= 0);
    assert(ptr || (len == 0));
    {// TODO : paranoid mode
        // Scan whole string (including terminator) for zero bytes
        // for TERM_NONE+TERM_READ, it's ok when no zero is found
        // for TERM_READ+TERM_ZERO, it's ok when zero is found at ptr+len
        void const * pos = memchr(ptr, '\0', len + (term >= STR_TERM_READ));
        assert(((term < STR_TERM_ZERO) && !pos)
            || ((term > STR_TERM_NONE) && (pos == (ptr + len))));
    }
    return STR_REF(ptr, len, term >= STR_TERM_READ);
}

inline StrRef str_ref_null()
{
    return STR_REF(NULL, 0, true);
}

inline StrRef str_ref_empty()
{
    return STR_REF("", 0, true);
}

inline StrRef str_ref_cstr(char const * str)
{
    size_t const len = str ? strlen(str) : 0;
    assert(len <= STR_LEN_MAX);
    return STR_REF(str, len, true);
}

// -- Queries --

#define STR_REF_PTR(ref) ((ref).ptr ? (ref).ptr : "")
#define STR_REF_LEN(ref) ((ref).flg>>1)

inline bool str_ref_is_null(StrRef ref)
{
    STR_REF_ASSERT(ref);
    return !ref.ptr;
}

inline bool str_ref_is_empty(StrRef ref)
{
    STR_REF_ASSERT(ref);
    return STR_REF_LEN(ref) == 0;
}

inline bool str_ref_is_read_term(StrRef ref)
{
    STR_REF_ASSERT(ref);
    return ref.flg&0x1;
}

inline bool str_ref_is_zero_term(StrRef ref)
{
    STR_REF_ASSERT(ref);
    return (ref.flg&0x1) && (STR_REF_PTR(ref)[STR_REF_LEN(ref)] == '\0');
}

inline bool str_ref_cmp_eq(StrRef a, StrRef b)
{
    STR_REF_ASSERT(a);
    STR_REF_ASSERT(b);
    StrLen const al = STR_REF_LEN(a);
    StrLen const bl = STR_REF_LEN(b);
    return (al == bl) && ((a.ptr == b.ptr) || (memcmp(a.ptr, b.ptr, al) == 0));
}

// -- Access --

inline StrLen str_ref_len(StrRef ref)
{
    STR_REF_ASSERT(ref);
    return STR_REF_LEN(ref);
}

inline char const * str_ref_ptr(StrRef ref)
{
    STR_REF_ASSERT(ref);
    return STR_REF_PTR(ref);
}

inline MemRef str_ref_mem(StrRef ref)
{
    STR_REF_ASSERT(ref);
    return mem_ref(ref.ptr, STR_REF_LEN(ref));
}

// -- Substrings --

inline StrRef STR_REF_TAIL(StrRef ref, StrLen n)
{
    StrLen const len = STR_REF_LEN(ref);
    return n <= len
        ? STR_REF(ref.ptr + n, len - n, ref.flg & 0x1)
        : str_ref_null();
}

inline StrRef str_ref_tail(StrRef ref, StrLen n)
{
    STR_REF_ASSERT(ref);
    assert(n >= 0);
    return STR_REF_TAIL(ref, n);
}

inline StrRef str_ref_init(StrRef ref, StrLen n)
{
    STR_REF_ASSERT(ref);
    assert(n >= 0);
    unsigned const len = STR_REF_LEN(ref);
    if((unsigned)n <= len)
    {
        ref.flg = ((len - n) << 1) | ((ref.flg & 0x1) || (n > 0));
    }
    else
    {
        ref = str_ref_null();
    }
    return ref;
}

inline StrRef str_ref_substr(StrRef ref, StrLen idx, StrLen len)
{
    STR_REF_ASSERT(ref);
    assert(idx >= 0);
    assert(len >= 0);
    ref = STR_REF_TAIL(ref, idx);
    StrLen ref_len = STR_REF_LEN(ref);
    if(len > ref_len)
    {
        len = ref_len;
    }
    return STR_REF(ref.ptr, ref_len, (ref.flg & 0x1) || (len < ref_len));
}

// -- Decomposition --

inline StrLen str_ref_find_c(StrRef ref, char c)
{
    STR_REF_ASSERT(ref);
    char const * const ptr = (char const *)memchr(ref.ptr, c, STR_REF_LEN(ref));
    assert(!ptr || (ptr >= ref.ptr));
    size_t const pos = ptr ? (StrLen)(ptr - ref.ptr) : STR_LEN_MAX;
    assert(pos <= STR_LEN_MAX);
    return pos;
}

inline StrRef STR_REF_CHOP_SPACES(StrRef ref)
{
    unsigned len = STR_REF_LEN(ref);
    bool read_term = ref.flg & 0x1;
    while((len > 0) && isspace(ref.ptr[len]))
    {
        read_term = true;
        --len;
    }
    ref.flg = (len << 1) + read_term;
    return ref;
} 

inline StrRef str_ref_chop_spaces(StrRef ref)
{
    STR_REF_ASSERT(ref);
    return STR_REF_CHOP_SPACES(ref);
} 

inline StrRef str_ref_trim_spaces(StrRef ref)
{
    STR_REF_ASSERT(ref);
    unsigned len = STR_REF_LEN(ref);
    while((len > 0) && isspace(ref.ptr[0]))
    {
        ++ref.ptr;
        --len;
    }
    ref.flg = (len << 1) + (ref.flg & 0x1);
    return STR_REF_CHOP_SPACES(ref);
} 

#ifdef __cplusplus
}//extern "C"

inline StrRef str_ref(StrRef const & ref) noexcept
{
    return ref;
}

#endif//__cplusplus
#endif//LIBSTR_REF_H_INCLUDED
