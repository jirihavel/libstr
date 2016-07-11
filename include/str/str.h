#ifndef LIBSTR_STR_H_INCLUDED
#define LIBSTR_STR_H_INCLUDED

#include <str/api.h>
#include <str/ref.h>

#ifdef __cplusplus
extern "C" {
#endif

#include <assert.h>
#include <limits.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

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
inline void str_str_init_weak(StrStr * str, char * ptr, int len, unsigned cap)
    __attribute__((nonnull(1)));
inline void str_str_init_const(StrStr * str, StrRef ref)
    __attribute__((nonnull));
//inline bool str_str_init_copy(StrStr * str, StrRef ref)
//    __attribute__((nonnull));

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

// -- Access --

inline int str_str_len(StrStr const * str)
    __attribute__((nonnull));
inline int str_str_cap(StrStr const * str)
    __attribute__((nonnull));
inline char const * str_str_ptr(StrStr const * str)
    __attribute__((nonnull));
inline char * str_str_ptr_mut(StrStr * str)
    __attribute__((nonnull));
inline StrRef str_str_ref(StrStr const * str)
    __attribute__((nonnull));

// -- Assignment --

// all _set_ functions assume initialized string

inline void str_str_set_null(StrStr * str)
    __attribute__((nonnull));
inline void str_str_set_empty(StrStr * str)
    __attribute__((nonnull));
inline void str_str_set_move(StrStr * restrict dst, StrStr * restrict src)
    __attribute__((nonnull));
inline void str_str_set_weak(StrStr * str, char * ptr, int len, unsigned cap)
    __attribute__((nonnull(1)));
inline void str_str_set_const(StrStr * str, StrRef ref)
    __attribute__((nonnull));

// -- Allocation --

bool str_str_alloc(StrStr * str, int cap, int len);

// -- Modification --

inline char const * str_str_cstr(StrStr * str)
    __attribute__((nonnull));
inline bool str_str_cat(StrStr * str, StrRef ref)
    __attribute__((nonnull));
bool str_str_fmt(StrStr * str, char const * fmt, ...)
    __attribute__((format(printf, 2, 3)));

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
 */
typedef enum StrTag_e
{
    STR_TAG_SSO = 0x0,// short strong string (ShortStringOptimization)
    STR_TAG_STR = 0x2,// long strong string
    STR_TAG_REF = 0x1 // long weak string
    //STR_TAG_??? = 0x3 unused for now
} StrTag;

/** \brief Long string representation.
 */
typedef struct StrRep_s
{
    char * ptr;
    unsigned len;
    unsigned cap;
} StrRep;

#define STR_SSO_CAP (sizeof(StrRep)-1)

typedef struct StrSSO_s
{
    char dat[STR_SSO_CAP];
    unsigned char len;
} StrSSO;

/** \brief Simple string.
 *
 * Can represent strings up to INT_MAX characters.
 */
typedef union StrStr_u
{
    StrRep rep;
    StrSSO sso;
} StrStr;

#define INT_BIT (sizeof(int)*CHAR_BIT)

// -- Internal functions --

inline StrTag str_str_get_tag(StrStr const * str)
    __attribute__((nonnull));
inline char const * str_str_get_ptr(StrStr const * str)
    __attribute__((nonnull));
inline int str_str_get_len(StrStr const * str)
    __attribute__((nonnull));
inline int str_str_get_cap(StrStr const * str)
    __attribute__((nonnull));
inline void str_str_set_tag(StrStr * str, StrTag tag)
    __attribute__((nonnull));
inline void str_str_set_len(StrStr * str, int len)
    __attribute__((nonnull));
inline void str_str_set_tag_len_cap(StrStr * str, StrTag tag, int len, int cap)
    __attribute__((nonnull));
inline void str_str_set_len_cap(StrStr * str, int len, int cap)
    __attribute__((nonnull));

inline StrTag str_str_get_tag(StrStr const * str)
{
    // uses sso structure even for long strings
    return (StrTag)(str->sso.len>>(CHAR_BIT-2));
}

inline int str_str_get_len(StrStr const * str)
{
    return str_str_get_tag(str) == STR_TAG_SSO
        ? STR_SSO_CAP - str->sso.len
        : str->rep.len & INT_MAX;
}

inline int str_str_get_cap(StrStr const * str)
{
    // 2nd msb of cap is stored as msb of len
    return str_str_get_tag(str) == STR_TAG_SSO
        ? STR_SSO_CAP
        : (str->rep.cap & (INT_MAX>>1)) | ((str->rep.len & (INT_MAX+1u))>>1);
}

inline char const * str_str_get_ptr(StrStr const * str)
{
    return str_str_get_tag(str) == STR_TAG_SSO
        ? str->sso.dat : str->rep.ptr;
}

#define STR_STR_ASSERT(p) do { assert((p));                                    \
    if(str_str_get_ptr(p)) { assert(str_str_get_len(p) >= 0);                  \
        if(str_str_get_cap(p) != 0) {                                          \
            assert(str_str_get_len(p) <= str_str_get_cap(p));                  \
            assert(str_str_get_ptr(p)[str_str_get_len(p)] == '\0'); } }        \
    else { assert(str_str_get_len(p) == 0); assert(str_str_get_cap(p) == 0); } \
} while(false)

inline void str_str_set_tag(StrStr * str, StrTag tag)
{
    assert(((int)tag >= 0) && ((int)tag <= 2));
    // Uses sso structure even for long strings
    str->sso.len = (tag<<(CHAR_BIT-2)) | (str->sso.len&(UCHAR_MAX>>2));
    // postcondition check
    assert(str_str_get_tag(str) == tag);
}

inline void str_str_set_len(StrStr * str, int len)
{
    assert(len >= 0);
    assert((str_str_get_cap(str) == 0) || (len <= str_str_get_cap(str)));
    if(str_str_get_tag(str) == STR_TAG_SSO)
        str->sso.len = STR_SSO_CAP - len;
    else
        str->rep.len = len | (str->rep.len & (INT_MAX+1u));
    // postcondition check
    assert(str_str_get_len(str) == len);
}

inline void str_str_set_tag_len_cap(StrStr * str, StrTag tag, int len, int cap)
{
    assert((tag == STR_TAG_STR) || (tag == STR_TAG_REF));
    assert(len >= 0); assert(cap >= 0);
    assert((cap == 0) || (len <= cap));
    str->rep.len = len | ((cap<<1) & (INT_MAX+1u));
    str->rep.cap = (cap & (INT_MAX>>1)) | (tag<<(INT_BIT-2));
    // postcondition check
    assert(str_str_get_tag(str) == tag);
    assert(str_str_get_len(str) == len);
    assert(str_str_get_cap(str) == cap);
}

inline void str_str_set_len_cap(StrStr * str, int len, int cap)
{
    str_str_set_tag_len_cap(str, str_str_get_tag(str), len, cap);
}

// -- Construction --

inline void str_str_init_null(StrStr * str)
{
    // init as weak reference with NULL pointer
    // - ptr=NULL, len=0, cap=REF 0...0
    memset(str, 0, sizeof(StrStr));
    str->sso.len = STR_TAG_REF<<(CHAR_BIT-2);
    // postcondition check
    assert(str_str_is_null(str));// PRE str ok
}

inline void str_str_init_empty(StrStr * str)
{
    // init as short string
    memset(str->sso.dat, 0, sizeof(StrStr));
    str->sso.len = STR_SSO_CAP; // set len=0
    // postcondition check
    assert(str_str_is_empty(str));// PRE str ok
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

inline void str_str_init_const(StrStr * str, StrRef ref)
{
    if(ref.len > INT_MAX)
    {
        str_str_init_null(str);
    }
    else
    {
        str->rep.ptr = (char*)ref.ptr;
        str_str_set_tag_len_cap(str, STR_TAG_REF, ref.len, 0);
        // postconditions
        assert(!str_str_is_mutable(str));// PRE str ok
    }
}

/** \brief Initialize string by copy of ref.
 *
 * \return false if ref is too big or allocation failed.
 */
//inline bool str_str_init_copy(StrStr * str, StrRef ref)
//{
//    return false;
//}

// -- Queries

inline bool str_str_is_null(StrStr const * str)
{
    STR_STR_ASSERT(str);
    return !str_str_get_ptr(str);
}

inline bool str_str_is_empty(StrStr const * str)
{
    STR_STR_ASSERT(str);
    return str_str_get_len(str) == 0;
}

inline bool str_str_is_mutable(StrStr const * str)
{
    STR_STR_ASSERT(str);
    return str_str_get_cap(str) > 0;
}

// -- Access --

inline int str_str_len(StrStr const * str)
{
    STR_STR_ASSERT(str);
    return str_str_get_len(str);
}

inline int str_str_cap(StrStr const * str)
{
    STR_STR_ASSERT(str);
    return str_str_get_cap(str);
}

/** \brief Read-only data pointer.
 *
 * \return nonnull pointer even for null string.
 */
inline char const * str_str_ptr(StrStr const * str)
{
    STR_STR_ASSERT(str);
    char const * ptr = str_str_get_ptr(str);
    return ptr ? ptr : "";
}

/** \brief Get string data pointer if the contents are mutable.
 *
 * \return NULL for immutable contents
 */
inline char * str_str_ptr_mut(StrStr * str)
{
    return str_str_is_mutable(str)// PRE str ok
        ? (char *)str_str_get_ptr(str) : NULL;
}

inline StrRef str_str_ref(StrStr const * str)
{
    STR_STR_ASSERT(str);
    return str_ref(str_str_get_ptr(str), str_str_get_len(str));
}


// -- Assignment --

inline void str_str_kill(StrStr * str)
{
    STR_STR_ASSERT(str);
    // free data if long strong string
    if(str_str_get_tag(str) == STR_TAG_STR)
        free(str->rep.ptr);
}

inline void str_str_set_null(StrStr * str)
{
    str_str_kill(str);// PRE  str ok
    str_str_init_null(str);// POST str ok
}

inline void str_str_set_empty(StrStr * str)
{
    if(str_str_is_mutable(str))// PRE str ok
        str_str_ptr_mut(str)[0] = '\0';
    str_str_set_len(str, 0);// POST len == x
}

inline void str_str_set_move(StrStr * restrict dst, StrStr * restrict src)
{
    if(dst != src)
    {
        str_str_kill(dst);
        str_str_init_move(dst, src);
    }
}

inline void str_str_set_weak(StrStr * str, char * ptr, int len, unsigned cap)
{
    str_str_kill(str);// PRE str ok
    str_str_init_weak(str, ptr, len, cap);// POST str ok
}

inline void str_str_set_const(StrStr * str, StrRef ref)
{
    str_str_kill(str);
    str_str_init_const(str, ref);
}

// -- Modifications --

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
    int const str_len = str_str_len(str);// PRE str ok
    char const * ptr = str_str_get_ptr(str);
    // if not null-terminated, reallocate
    bool ok = ptr && (ptr[str_len] == '\0');
    if(!ok)
    {
        ok = str_str_alloc(str, -1, str_len);// puts zero-term
        ptr = str_str_get_ptr(str);
    }
    // ensure string does not contain '\0'
    return ok && !memchr(ptr, '\0', str_len) ? ptr : NULL;
}

inline bool str_str_cat(StrStr * str, StrRef ref)
{
    int const str_len = str_str_len(str);// PRE str ok
    STR_REF_ASSERT(&ref);
    // str->len + ref.len <= INT_MAX (0 <= str->len <= INT_MAX)
    bool const ok = (ref.len <= (size_t)(INT_MAX-str_len))
        && str_str_alloc(str, str_len+ref.len, INT_MAX);
    if(ok)
    {
        char * ptr = str_str_ptr_mut(str);
        memcpy(ptr+str_len, ref.ptr, ref.len);
        ptr[str_len+ref.len] = '\0';
        str_str_set_len(str, str_len+ref.len);
    }
    return ok;
}

#ifdef __cplusplus
}
#endif

#endif//LIBSTR_STR_H_INCLUDED
