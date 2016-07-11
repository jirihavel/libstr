#ifndef LIBSTR_FORMAT_H_INCLUDED
#define LIBSTR_FORMAT_H_INCLUDED

#include <str/api.h>
#include <str/ref.h>

#ifdef __cplusplus
extern "C" {
#endif

#include <limits.h>
#include <stddef.h>

#include <sys/types.h> // ssize_t

void str_rst_dst(char ** dst, size_t * cap);
size_t str_add_char(char ** dst, size_t * cap, char c);
ssize_t str_fmt(char ** dst, size_t * capacity, char const * fmt, ...);

// -- Copy --

size_t str_cpy(char ** dst, size_t * cap, StrRef ref);
size_t str_cpy_tolower(char ** dst, size_t * cap, StrRef ref);
size_t str_cpy_toupper(char ** dst, size_t * cap, StrRef ref);

// -- Base16 encoding --

ssize_t str_dec_b16(char ** dst, size_t * cap, StrRef ref);
size_t str_enc_b16(char ** dst, size_t * cap, StrRef ref);
size_t str_enc_B16(char ** dst, size_t * cap, StrRef ref);

// -- URI encoding --

size_t str_enc_uri(char ** dst, size_t * cap, StrRef ref);

// -- WWW form encoding --

size_t str_enc_www_form_component_size(StrRef ref);
size_t str_enc_www_form_component(char ** dst, size_t * cap, StrRef ref);

struct StrKeyValRef_s;
typedef struct StrKeyValRef_s StrKeyValRef;

inline StrKeyValRef str_kvr(StrRef key, StrRef val);
inline StrRef str_kvr_key(StrKeyValRef const * kvr);
inline StrRef str_kvr_val(StrKeyValRef const * kvr);

size_t str_enc_www_form_size(StrKeyValRef const * data, size_t cnt);
size_t str_enc_www_form(char ** dst, size_t * cap, StrKeyValRef const * data, size_t cnt);

// -- Data URI encoding --

size_t str_enc_data_uri_size(StrRef mime, StrRef data);
size_t str_enc_data_uri(char ** dst, size_t * cap, StrRef mime, StrRef data);

// -- Implementation --

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

#endif//LIBSTR_FORMAT_H_INCLUDED
