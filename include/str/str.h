#ifndef LIBSTR_STR_H_INCLUDED
#define LIBSTR_STR_H_INCLUDED

#include <str/ref.h>

#ifdef __cplusplus
# include <cstdint>
# include <cstdlib>
extern "C" {
#else
# include <stdint.h>
# include <stdlib.h>
#endif

// weak, strong (reference or hold data?)
// - weak can be automatically promoted to strong
// - strong is always mutable
// - weak can be immutable (references to other strings)
//   - or mutable (use small stack array and reallocate if needed)
// empty, null (special case)
// -> strong
// ptr, sso

// cases :
// null
// - special case of empty (REF, ptr=nullptr, len=0, cap=0)
// - zt
// empty
// - (REF[_RT]/STR/SSO, ptr="", len=0, cap=0)
// - zt
// const
// - (REF[_RT], ptr=..., len=..., cap=0)
// - ?t
// weak
// - (REF_RT, ptr=...(cap+1B), len=..., cap=...)
// - cap=0 (single byte array) doesn't make sense
// - zt
// strong
// - (STR, ptr=malloc(cap+1), len=..., cap=...)
// - (SSO, ...)
// - 1 additional byte is allocated for terminator
// - zt
//
// == Layout ==
// REP (REF[_RT]/STR)
// - ppppppppffffffff
// - ptr 4/8B
// - flg uint64_t (2*31b + 2b)
//   - tag (2b)  = flg >> 62 (two highest bits, correspond to last byte of SSO struct)
//   - len (31b) = flg & INT_MAX
//   - cap (31b) = (flg >> 31) & INT_MAX
// SSO
// - xxxxxxxxxxxxxxxy
// - dat sizeof(Rep)-1B (STR_SSO_CAP) (11/15B)
// - flg 1B uint8_t
//   - tag (2b) = flg >> 6
//   - len (6b) = STR_SSO_CAP - flg (flg must be 0 for len=STR_SSO_CAP and tag is 0 in SSO mode)
//   - cap = STR_SSO_CAP

union StrStr_u;
typedef union StrStr_u StrStr;

// -- Initialization --

// all _init_ functions assume unitialized string
// - initialized string must be killed first
// - otherwise any allocated memory would leak

inline void str_str_init_null(StrStr * str)
    __attribute__((nonnull));
inline void str_str_init_empty(StrStr * str)
    __attribute__((nonnull));
inline void str_str_init_move(StrStr * restrict dst, StrStr * restrict src)
    __attribute__((nonnull));
inline void str_str_init_const(StrStr * str, StrRef ref)
    __attribute__((nonnull));
inline bool str_str_init_copy(StrStr * str, StrRef ref)
    __attribute__((nonnull));
inline void str_str_init_weak(StrStr * str, char * ptr, StrLen len, unsigned cap)
    __attribute__((nonnull(1)));

// -- Deinitialization --

inline void str_str_kill(StrStr * str)
    __attribute__((nonnull));

// -- Queries --

inline bool str_str_is_null(StrStr const * str)
    __attribute__((nonnull));
inline bool str_str_is_empty(StrStr const * str)
    __attribute__((nonnull));
inline bool str_str_is_mutable(StrStr const * str)
    __attribute__((nonnull));
inline bool str_str_is_strong(StrStr const * str)
    __attribute__((nonnull));

// -- Access --

inline StrLen str_str_len(StrStr const * str)
    __attribute__((nonnull));
inline StrLen str_str_cap(StrStr const * str)
    __attribute__((nonnull));
inline char const * str_str_ptr(StrStr const * str)
    __attribute__((nonnull));
inline StrRef str_str_ref(StrStr const * str)
    __attribute__((nonnull));
inline char * str_str_ptr_mut(StrStr * str)
    __attribute__((nonnull));

// -- Assignment --

// all _set_ functions assume initialized string

inline void str_str_set_null(StrStr * str)
    __attribute__((nonnull));
inline void str_str_set_empty(StrStr * str)
    __attribute__((nonnull));
inline void str_str_set_move(StrStr * restrict dst, StrStr * restrict src)
    __attribute__((nonnull));
inline void str_str_set_const(StrStr * str, StrRef ref)
    __attribute__((nonnull));
inline bool str_str_set_copy(StrStr * str, StrRef ref)
    __attribute__((nonnull));
inline void str_str_set_weak(StrStr * str, char * ptr, int len, unsigned cap)
    __attribute__((nonnull(1)));

// -- Allocation --

bool str_str_alloc(StrStr * str, StrLen cap, StrLen len);

// -- Modification --

inline char const * str_str_cstr(StrStr * str)
    __attribute__((nonnull));
inline bool str_str_cat(StrStr * str, StrRef ref)
    __attribute__((nonnull));
//bool str_str_fmt(StrStr * str, char const * fmt, ...)
//    __attribute__((format(printf, 2, 3)));

// Behavior :
//
// null string
// - special case of empty string
// - behaves as empty except for query for nullness
//
// empty string
// - length = 0, no other requirements (storage, capacity, termination)
//
// zero-term, read-term, none-term
// - terminator is either zero, can be read and checked whether is zero or can't be accessed
// - e.g.
// -- any substring of zero-term or read-term string is read-term
// -- substring of none-term can be read-term if it doesn't include last char
//
// Internals :
//
// short x long string
// - data stored inside x outside
//
// strong x weak string
// - data owned x referenced by the string
// - data lifetime is programmers concern in case of weak strings
// - strong strings are always zero-term
// -- allocates cap+1 bytes
// - weak strings are ?-term (what here?)
// -- cap == 0 (reference to literal-like stuff), read flag is in tag
// -- cap >  0 -> zero-term (must point to array of cap+1 bytes)
//


// Layout
//4+4+4 :
//- ptr, cap, len
//- cap - 31b, 1b available
//- len - 31b, 1b available
//- tag = cap>>30
//- length = len&INT_MAX
//- capacity = cap&(INT_MAX>>1) + len&(INT_MAX+1)
//....,....,...x : (x is MSB of cap, 2nd MSB of cap is in MSB of len)
//- 00...... = sso (two most significant bits)
//-- len = 11-x, for 11 it is 0 and zero terminator
//- ......XX = !sso :
//-- ref - cap == 0, len >= 0
//-- out - cap >= 1, len <= x
//-- own - cap >= 1, len <= x
//- sso 
//- ref 
//- own
//- out
//-- ...0 = 11
//
//8+4+4 :
//- 15+1

// variants
// short x long
// const x locked x mutable
// weak x strong
// zero-terminated x terminator readable x terminator inaccessible
// tag : 2b, 
// - tag : 00 - short (mutable=0/locked=1 as msb of len, remains 2^5 = 32)
// - tag : XY - long
//
// short :
// - implies strong, mutable, zero-terminated
//
// long :
// - cap == 0 - const
// - cap != 0 - not const
//
// const :
// - implies weak, immutable, ?terminator-readable
//
// not const - tag!=0, cap != 0
// - tag : 10 - strong
// - tag : X1 - weak
//
// strong
// - implies mutable, zero-terminated
//
// weak
// - 
//
// - tag : 10 - strong (mutable=0/locked=1 as lsb of ptr, malloc is aligned)
// - tag : 01 - weak mutable
// - tag : 11 - weak locked
//
// short (tag == 00) : implies strong, !const, add one tag bit for locked/mutable. 
// tag != 00 - long
// - cap == 0 - weak, const
// - cap != 0 - locked/mutable, weak/strong

/** \brief Type of string contents.
 *
 * All types of string should behave identically from the outside.
 *
 * Checks :
 * - tag != STR_TAG_SSO -> rep
 * - tag >= STR_TAG_REF -> ref
 */
typedef enum StrTag_e
{
    STR_TAG_SSO = 0x0,
    STR_TAG_STR = 0x1,
    STR_TAG_REF = 0x2,
    STR_TAG_REF_RT = 0x3,
} StrTag;

/** \brief Long string representation.
 */
typedef struct StrRep_s
{
    char * ptr;
    uint64_t flg;
} StrRep;

#define STR_SSO_CAP (sizeof(StrRep)-1)

typedef struct StrSSO_s
{
    char dat[STR_SSO_CAP];
    uint8_t flg;
} StrSSO;

/** \brief Not so simple string.
 *
 * Can represent strings up to STR_LEN_MAX characters.
 *
 * Can either hold strings by value (strong) or by reference (weak).
 *
 *
 * Uses small string optimization. On x64 can hold up to 15B strings without allocation.
 */
typedef union StrStr_u
{
    StrRep rep;
    StrSSO sso;
} StrStr;

#define INT_BIT (sizeof(int)*CHAR_BIT)

// -- Internal functions --

inline StrTag STR_STR_TAG(StrStr const * str)
    __attribute__((nonnull, pure));
inline char const * STR_STR_PTR_OPT(StrStr const * str)
    __attribute__((nonnull, pure));
inline char const * STR_STR_PTR(StrStr const * str)
    __attribute__((nonnull, pure));
inline StrLen STR_STR_LEN(StrStr const * str)
    __attribute__((nonnull, pure));
inline StrLen STR_STR_CAP(StrStr const * str)
    __attribute__((nonnull, pure));

inline StrTag STR_STR_TAG(StrStr const * str)
{
    // uses sso structure even for long strings
    return (StrTag)(str->sso.flg>>(CHAR_BIT-2));
}

inline char const * STR_STR_PTR_OPT(StrStr const * str)
{
    return STR_STR_TAG(str) == STR_TAG_SSO
        ? str->sso.dat
        : str->rep.ptr;
}

inline char const * STR_STR_PTR(StrStr const * str)
{
    return STR_STR_TAG(str) == STR_TAG_SSO
        ? str->sso.dat
        : str->rep.ptr
            ? str->rep.ptr
            : "";
}

inline StrLen STR_STR_LEN(StrStr const * str)
{
    return STR_STR_TAG(str) == STR_TAG_SSO
        ? STR_SSO_CAP - str->sso.flg
        : str->rep.flg & INT_MAX;
}

inline StrLen STR_STR_CAP(StrStr const * str)
{
    return STR_STR_TAG(str) == STR_TAG_SSO
        ? STR_SSO_CAP
        : (str->rep.flg >> 31) & INT_MAX;
}

inline uint64_t STR_STR_REP_FLG(StrTag tag, StrLen len, StrLen cap)
{
    return ((uint64_t)tag << 62) | ((uint64_t)cap << 31) | len;
}

#define STR_STR_ASSERT(p) do { assert((p));                            \
    if(STR_STR_PTR_OPT(p)) { assert(STR_STR_LEN(p) >= 0); }            \
    else { assert(STR_STR_LEN(p) == 0); assert(STR_STR_CAP(p) == 0); } \
} while(false)

#if 0
#define STR_STR_ASSERT(p) do { assert((p));                                    \
    if(str_str_get_ptr(p)) { assert(str_str_get_len(p) >= 0);                  \
        if(str_str_get_cap(p) != 0) {                                          \
            assert(str_str_get_len(p) <= str_str_get_cap(p));                  \
            assert(str_str_get_ptr(p)[str_str_get_len(p)] == '\0'); } }        \
    else { assert(str_str_get_len(p) == 0); assert(str_str_get_cap(p) == 0); } \
} while(false)
#endif

// -- Construction --

inline void str_str_init_null(StrStr * str)
{
    assert(str);
    // init as weak reference with NULL pointer
    str->rep.ptr = NULL;
    str->rep.flg = STR_STR_REP_FLG(STR_TAG_REF_RT, 0, 0);
}

inline void str_str_init_empty(StrStr * str)
{
    // init as short string
    memset(str->sso.dat, 0, STR_SSO_CAP);
    str->sso.flg = STR_SSO_CAP;// set len=0
}

inline void str_str_init_move(StrStr * restrict dst, StrStr * restrict src)
{
    STR_STR_ASSERT(src);
    if(dst != src) // prevent self move
    {
        memcpy(dst, src, sizeof(StrStr));
        // src was moved, now we assume it's not initialized
        str_str_init_null(src);// POST src null
    }
}

inline void str_str_init_const(StrStr * str, StrRef ref)
{
    STR_REF_ASSERT(ref);
    str->rep.ptr = (char*)ref.ptr;// preserve null
    str->rep.flg = STR_STR_REP_FLG(
        str_ref_is_read_term(ref) ? STR_TAG_REF_RT : STR_TAG_REF,
        STR_REF_LEN(ref), 0);
}

inline bool str_str_init_copy(StrStr * str, StrRef ref)
{
    STR_REF_ASSERT(ref);
    StrLen const len = STR_REF_LEN(ref);
    if(len <= (StrLen)STR_SSO_CAP)
    {
        memcpy(str->sso.dat, STR_REF_PTR(ref), len);
        str->sso.dat[len] = '\0';
        str->sso.flg = STR_SSO_CAP - len;
    }
    else
    {
        str->rep.ptr = (char*)malloc(len + 1u);
        if(!str->rep.ptr)
        {
            return false;
        }
        memcpy(str->rep.ptr, STR_REF_PTR(ref), len);
        str->rep.ptr[len] = '\0';
        str->rep.flg = STR_STR_REP_FLG(STR_TAG_STR, len, len);
    }
    return true;
}

inline void str_str_init_weak(StrStr * str, char * ptr, StrLen len, unsigned cap)
{
    str->rep.ptr = ptr;
    str->rep.flg = STR_STR_REP_FLG(STR_TAG_REF_RT, len, cap-1);
}

#if 0

inline void str_str_init_weak(StrStr * str, char * ptr, int len, unsigned cap)
{
    // !ptr -> len==0
    // cap <= INT_MAX characters + terminator
    //if((!ptr && (len != 0)) || (cap > (INT_MAX+1u)))
    //    goto error;
    assert(ptr || (len == 0)); // !ptr implies len==0
    assert(cap <= (INT_MAX+1u)); // 
    // count characters if len is negative
    if(len < 0)
    {
        size_t size = strlen(ptr);
        if(size > INT_MAX)
        {
            str_str_init_null(str);
            return;
        }
        len = size;
        // surely zero-term
    }
    assert(len >= 0);
    // zero terminate if mutable
    if(cap > 0)
    {
        assert((unsigned)len < cap);
        ptr[len] = '\0';
        --cap; // keep one byte for terminator
        // surely zero-term
    }
    str->rep.ptr = ptr;
    str_str_set_tag_len_cap(str, STR_TAG_REF, len, cap);
    // postcondition check
    STR_STR_ASSERT(str);
}

#endif
// -- Queries

inline bool str_str_is_null(StrStr const * str)
{
    STR_STR_ASSERT(str);
    return !STR_STR_PTR_OPT(str);
}

inline bool str_str_is_empty(StrStr const * str)
{
    STR_STR_ASSERT(str);
    return STR_STR_LEN(str) == 0;
}

inline bool STR_STR_IS_MUTABLE(StrStr const * str)
{
    return STR_STR_CAP(str) > 0;
}

inline bool str_str_is_mutable(StrStr const * str)
{
    STR_STR_ASSERT(str);
    return STR_STR_IS_MUTABLE(str);
}

inline bool str_str_is_strong(StrStr const * str)
{
    STR_STR_ASSERT(str);
    return STR_STR_TAG(str) < STR_TAG_REF;
}

inline bool STR_STR_IS_READ_TERM(StrStr const * str)
{
    return STR_STR_TAG(str) != STR_TAG_REF;
}

inline bool STR_STR_IS_ZERO_TERM(StrStr const * str)
{
    StrTag const tag = STR_STR_TAG(str);
    return (tag < STR_TAG_REF)
        || ((tag == STR_TAG_REF_RT) && (str->rep.ptr[STR_STR_LEN(str)] == '\0'));
}

// -- Access --

inline StrLen str_str_len(StrStr const * str)
{
    STR_STR_ASSERT(str);
    return STR_STR_LEN(str);
}

inline StrLen str_str_cap(StrStr const * str)
{
    STR_STR_ASSERT(str);
    return STR_STR_CAP(str);
}

/** \brief Read-only data pointer.
 *
 * \return nonnull pointer even for null string.
 */
inline char const * str_str_ptr(StrStr const * str)
{
    STR_STR_ASSERT(str);
    return STR_STR_PTR(str);
}

inline StrRef str_str_ref(StrStr const * str)
{
    STR_STR_ASSERT(str);
    return STR_REF(STR_STR_PTR_OPT(str), STR_STR_LEN(str), STR_STR_IS_READ_TERM(str));
}

inline char * STR_STR_PTR_MUT(StrStr * str)
{
    if(!STR_STR_IS_MUTABLE(str))
    {
        StrStr aux;
        if(!str_str_init_copy(&aux, str_str_ref(str)))
        {
            return NULL;
        }
        // only mutable strings need release, just overwrite str
        *str = aux;
    }
    char const * ptr = STR_STR_PTR_OPT(str);
    assert(ptr);
    return (char*)ptr;
}

/** \brief Get pointer to mutable content.
 *
 * If the string is immutable, the function tries to make it mutable.
 *
 * \return NULL if turning mutable failed (allocation failure).
 */
inline char * str_str_ptr_mut(StrStr * str)
{
    STR_STR_ASSERT(str);
    return STR_STR_PTR_MUT(str);
}

/** \brief Get pointer to zero-terminated string contents.
 *
 * The function ensures ptr[0..len-1] != '\0' and ptr[len] == '\0'
 *
 * O(N) If this is an issue, cstrings are bad idea anyway.
 *
 * \return NULL if string contains '\0' or reallocation fails.
 */
inline char const * str_str_cstr(StrStr * str)
{
    STR_STR_ASSERT(str);
    return STR_STR_IS_ZERO_TERM(str)
        ? STR_STR_PTR(str)
        : STR_STR_PTR_MUT(str);
}

// -- ... --

inline void STR_STR_KILL(StrStr * str)
{
    if(STR_STR_TAG(str) == STR_TAG_STR)
    {
        free(str->rep.ptr);
    }
}

inline void str_str_kill(StrStr * str)
{
    STR_STR_ASSERT(str);
    STR_STR_KILL(str);
}

// -- Assignment --

inline void str_str_set_null(StrStr * str)
{
    STR_STR_ASSERT(str);
    STR_STR_KILL(str);
    str_str_init_null(str);
}

inline void str_str_set_empty(StrStr * str)
{
    STR_STR_ASSERT(str);
    STR_STR_KILL(str);
    str_str_init_empty(str);
}

inline void str_str_set_move(StrStr * restrict dst, StrStr * restrict src)
{
    STR_STR_ASSERT(dst);
    STR_STR_ASSERT(src);
    if(dst != src)
    {
        STR_STR_KILL(dst);
        str_str_init_move(dst, src);
    }
}

inline void str_str_set_const(StrStr * str, StrRef ref)
{
    STR_STR_ASSERT(str);
    STR_STR_KILL(str);
    str_str_init_const(str, ref);
}

inline bool str_str_set_copy(StrStr * str, StrRef ref)
{
    STR_STR_ASSERT(str);
    StrStr aux;
    if(!str_str_init_copy(&aux, ref))
    {
        return false;
    }
    STR_STR_KILL(str);
    *str = aux;
    return true;
}

inline void str_str_set_weak(StrStr * str, char * ptr, int len, unsigned cap)
{
    STR_STR_ASSERT(str);
    STR_STR_KILL(str);
    str_str_init_weak(str, ptr, len, cap);
}

#if 0

// -- Modifications --

inline bool str_str_cat(StrStr * str, StrRef ref)
{
    int const str_len = str_str_len(str);// PRE str ok
    STR_REF_ASSERT(ref);
    StrLen ref_len = STR_REF_LEN(ref);
    // str->len + ref.len <= INT_MAX (0 <= str->len <= INT_MAX)
    bool const ok = (ref_len <= (size_t)(INT_MAX-str_len))
        && str_str_alloc(str, str_len+ref_len, INT_MAX);
    if(ok)
    {
        char * ptr = str_str_ptr_mut(str);
        memcpy(ptr+str_len, ref.ptr, ref_len);
        ptr[str_len+ref_len] = '\0';
        str_str_set_len(str, str_len+ref_len);
    }
    return ok;
}
#endif
#ifdef __cplusplus
}//extern "C"

inline StrRef str_ref(StrStr const & str) noexcept
{
    return str_str_ref(&str);
}

namespace str {

class string
{
public :
    using value_type = char;
    using size_type = StrLen;

    static constexpr size_type npos = STR_LEN_MAX;

    string()
    {
        str_str_init_null(&m_str);
    }
    string(string && str)
    {
        str_str_init_move(&m_str, &str.m_str);
    }
    //string(string const & str);
    template<typename StrRef>
    string(StrRef const & ref, size_type pos = 0, size_type len = npos)
    {
        str_str_init_copy(&m_str, str_ref_substr(str_ref(ref), pos, len));
    }
    string(char const * s)
    {
        str_str_init_copy(&m_str, str_ref_cstr(s));
    }
    string(char const * s, size_type n)
    {
        str_str_init_copy(&m_str, str_ref(s, n));
    }
    //string(size_type n, char c);
    //template<typename InputIterator>
    //string(InputIterator beg, InputIterator end);
    //string(std::initializer_list<char> il);

    ~string() noexcept;

    string & operator=(string str) noexcept;//but parameter copying may throw

    bool empty() const
    {
        return STR_STR_LEN(&m_str) == 0;
    }
    size_type size() const
    {
        return STR_STR_LEN(&m_str);
    }
    size_type max_size() const
    {
        return STR_LEN_MAX;
    }
    size_type capacity() const
    {
        return STR_STR_CAP(&m_str);
    }
    void clear();
    void shrink_to_fit();
    void resize(size_type n);
    void reserve(size_type n);
    
    char const & back() const;
    char       & back();
    char const & front() const;
    char       & front();
    char const & at(size_type i) const;
    char       & at(size_type i);
    char const & operator[](size_type i) const;
    char       & operator[](size_type i);
private :
    StrStr m_str;
};

}//namespace str
#endif//__cplusplus
#endif//LIBSTR_STR_H_INCLUDED
