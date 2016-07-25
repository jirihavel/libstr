#ifndef LIBSTR_KEY_VALUE_REFERENCE_H_INCLUDED
#define LIBSTR_KEY_VALUE_REFERENCE_H_INCLUDED

#include <str/api.h>
#include <str/ref.h>

#ifdef __cplusplus
extern "C" {
#endif

// -- Interface --

struct StrKeyValRef_s;
typedef struct StrKeyValRef_s StrKeyValRef;

inline StrKeyValRef str_kvr(StrRef key, StrRef val);
inline StrRef str_kvr_key(StrKeyValRef const * kvr);
inline StrRef str_kvr_val(StrKeyValRef const * kvr);

// -- Implementation --

#include <limits.h>

/** \brief Key-value pair of string references.
 */
struct StrKeyValRef_s
{
    char const * key;
    char const * val;
    unsigned key_len;
    unsigned val_len;
};

inline StrKeyValRef str_kvr(StrRef key, StrRef val)
{
    assert(key.len <= UINT_MAX);
    assert(val.len <= UINT_MAX);
    return (StrKeyValRef) { .key = key.ptr, .val = val.ptr,
        .key_len = (unsigned)key.len, .val_len = (unsigned)val.len };
}

inline StrRef str_kvr_key(StrKeyValRef const * kvr)
{
    return str_ref(kvr->key, kvr->key_len);
}

inline StrRef str_kvr_val(StrKeyValRef const * kvr)
{
    return str_ref(kvr->val, kvr->val_len);
}

#ifdef __cplusplus
}
#endif

#endif//LIBSTR_KEY_VALUE_REFERENCE_H_INCLUDED 
