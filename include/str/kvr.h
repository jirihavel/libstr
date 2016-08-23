#ifndef LIBSTR_KEY_VALUE_REFERENCE_H_INCLUDED
#define LIBSTR_KEY_VALUE_REFERENCE_H_INCLUDED

#include <str/api.h>
#include <str/ref.h>

#ifdef __cplusplus
extern "C" {
#endif

#include <assert.h>
#include <limits.h>

// -- Interface --

struct StrKeyValRef_s;
typedef struct StrKeyValRef_s StrKeyValRef;

inline StrKeyValRef str_kvr(StrRef key, StrRef val);
inline StrRef str_kvr_key(StrKeyValRef kvr);
inline StrRef str_kvr_val(StrKeyValRef kvr);

// -- Implementation --

/** \brief Key-value pair of string references.
 *
 * On architectures such as x64, it has size of three pointers instead of four as two StrRefs.
 */
struct StrKeyValRef_s
{
    char const * key;
    char const * val;
    unsigned key_flg;
    unsigned val_flg;
};

inline StrKeyValRef str_kvr(StrRef key, StrRef val)
{
    STR_REF_ASSERT(key);
    STR_REF_ASSERT(val);
    return (StrKeyValRef) { .key = key.ptr, .val = val.ptr,
        .key_flg = key.flg, .val_flg = val.flg };
}

inline StrRef str_kvr_key(StrKeyValRef kvr)
{
    return (StrRef) { .ptr = kvr.key, .flg = kvr.key_flg };
}

inline StrRef str_kvr_val(StrKeyValRef kvr)
{
    return (StrRef) { .ptr = kvr.val, .flg = kvr.val_flg };
}

#ifdef __cplusplus
}
#endif

#endif//LIBSTR_KEY_VALUE_REFERENCE_H_INCLUDED 
