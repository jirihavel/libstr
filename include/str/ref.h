#ifndef LIBSTR_REF_H_INCLUDED
#define LIBSTR_REF_H_INCLUDED

#include <str/mem.h>

#ifdef __cplusplus
# include <cctype>
# include <cstring>
extern "C" {
#else
# include <ctype.h>
# include <string.h>
#endif

// -- Interface --

struct StrRef_s;
typedef struct StrRef_s StrRef;

// -- Construction --

inline StrRef str_ref_null()
    __attribute__((const));
inline StrRef str_ref_empty()
    __attribute__((const));
inline StrRef str_ref_cstr(char const * str)
    __attribute__((pure));
inline StrRef str_ref(char const * ptr, StrLen len, StrTerm term STR_DEFAULT(STR_TERM_NONE))
    __attribute__((pure));

// -- Queries --

inline bool str_ref_is_null(StrRef ref)
    __attribute__((const));
inline bool str_ref_is_empty(StrRef ref)
    __attribute__((const));
inline bool str_ref_is_read_term(StrRef ref)
    __attribute__((const));
inline bool str_ref_is_zero_term(StrRef ref)
    __attribute__((const));

inline bool str_ref_cmp_eq(StrRef a, StrRef b)
    __attribute__((const));
//inline bool str_ref_cmp_lt(StrRef a, StrRef b)
//    __attribute__((const));
//inline bool str_ref_cmp_le(StrRef a, StrRef b)
//    __attribute__((const));

// -- Access --

inline char const * str_ref_ptr(StrRef ref)
    __attribute__((const));
inline StrLen str_ref_len(StrRef ref)
    __attribute__((const));
inline MemRef str_ref_mem(StrRef ref)
    __attribute__((const));

// -- Substrings --

inline StrRef str_ref_tail(StrRef ref, StrLen n STR_DEFAULT(1))
    __attribute__((const));
inline StrRef str_ref_init(StrRef ref, StrLen n STR_DEFAULT(1))
    __attribute__((const));
inline StrRef str_ref_substr(StrRef ref, StrLen idx, StrLen len STR_DEFAULT(STR_LEN_MAX))
    __attribute__((const));

// -- Decomposition --

inline StrLen str_ref_find_c(StrRef ref, int c)
    __attribute__((const));
StrRef str_ref_word_c(StrRef * ref, int c)
    __attribute__((nonnull));

StrRef str_ref_chop(StrRef ref, int (*is_char)(int));
StrRef str_ref_trim(StrRef ref, int (*is_char)(int));

inline StrRef str_ref_chop_spaces(StrRef ref);
inline StrRef str_ref_trim_spaces(StrRef ref);

//----------------------
// -- Implementation --
//----------------------

/** \brief Weak reference to string.
 *
 * It is safely copyable so it can be passed by value.
 *
 * !ptr -> null string (len == 0, special case of empty string)
 * - null strings should behave exactly as empty
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

#define STR_REF_PTR(ref) ((ref).ptr ? (ref).ptr : "")
#define STR_REF_LEN(ref) ((ref).flg>>1)

/** \brief Check all invariants.
 *
 * TODO : paranoid mode ON/OFF
 */
#define STR_REF_ASSERT(ref) do {                            \
    unsigned const len = STR_REF_LEN(ref);                  \
    assert(len <= STR_LEN_MAX);                             \
    assert((ref).ptr || (len == 0));                        \
    if((ref).ptr) {                                         \
        bool const rt = (ref).flg & 0x1;                    \
        void const * pos = memchr((ref).ptr, '\0', len+rt); \
        assert(!pos || (rt && (pos == (ref).ptr + len))); } \
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
    if(ptr)
    {// TODO : paranoid mode ON/OFF
        // Scan whole string (including terminator) for zero bytes
        // for TERM_NONE+TERM_READ, it's ok when no zero is found
        // for TERM_READ+TERM_ZERO, it's ok when zero is found at ptr+len
        void const * pos = memchr(ptr, '\0', (unsigned)len + (term >= STR_TERM_READ));
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

inline StrRef STR_REF_SUBSTR(StrRef ref, StrLen idx, StrLen len)
{
    ref = STR_REF_TAIL(ref, idx);
    StrLen ref_len = STR_REF_LEN(ref);
    if(len > ref_len)
    {
        len = ref_len;
    }
    return STR_REF(ref.ptr, ref_len, (ref.flg & 0x1) || (len < ref_len));
}

inline StrRef str_ref_substr(StrRef ref, StrLen idx, StrLen len)
{
    STR_REF_ASSERT(ref);
    assert(idx >= 0);
    assert(len >= 0);
    return STR_REF_SUBSTR(ref, idx, len);
}

// -- Decomposition --

inline StrLen str_ref_find_c(StrRef ref, int c)
{
    STR_REF_ASSERT(ref);
    char const * const ptr = (char const *)memchr(STR_REF_PTR(ref), c, STR_REF_LEN(ref));
    assert(!ptr || (ref.ptr && (ptr >= ref.ptr)));
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

inline bool str_ref_match_prefix(StrRef ref, StrRef prefix, StrRef * rest)
{
    STR_REF_ASSERT(ref);
    STR_REF_ASSERT(prefix);
    StrLen const pre_len = STR_REF_LEN(prefix);
    if(((StrLen)STR_REF_LEN(ref) >= pre_len) &&
       (memcmp(STR_REF_PTR(ref), STR_REF_PTR(prefix), pre_len) == 0))
    {
        if(rest)
        {
            *rest = STR_REF_SUBSTR(ref, pre_len, STR_LEN_MAX);
        }
        return true;
    }
    return false;
}

#ifdef __cplusplus
}//extern "C"

inline StrRef str_ref(StrRef const & ref) noexcept
{
    STR_REF_ASSERT(ref);
    return ref;
}

inline MemRef mem_ref(StrRef const & ref) noexcept
{
    STR_REF_ASSERT(ref);
    return MEM_REF(ref.ptr, STR_REF_LEN(ref));
}


namespace std {

template<typename Char, typename Traits> class basic_string_view;

template<typename Traits>
inline StrRef str_ref(basic_string_view<char, Traits> const & str) noexcept
{
    assert(str.size() <= STR_LEN_MAX);
    return STR_REF(str.data(), str.size(), false);
}

template<typename Char, typename Traits>
inline MemRef mem_ref(basic_string_view<Char, Traits> const & str) noexcept
{
    return MEM_REF(str.data(), sizeof(Char)*str.size());
}

template<typename Char, typename Traits, typename Alloc> class basic_string;

template<typename Traits, typename Alloc>
inline StrRef str_ref(basic_string<char, Traits, Alloc> const & str) noexcept
{
    assert(str.size() <= STR_LEN_MAX);
    return STR_REF(str.data(), str.size(), true);
}

template<typename Char, typename Traits, typename Alloc>
inline MemRef mem_ref(basic_string<Char, Traits, Alloc> const & str) noexcept
{
    return MEM_REF(str.data(), sizeof(Char)*str.size());
}

}//namespace std

#endif//__cplusplus
#endif//LIBSTR_REF_H_INCLUDED
