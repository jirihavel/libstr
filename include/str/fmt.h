#ifndef LIBSTR_FORMAT_H_INCLUDED
#define LIBSTR_FORMAT_H_INCLUDED

#include <str/kvr.h>

#include <sys/types.h> // ssize_t

#ifdef __cplusplus
extern "C" {
#endif

void str_rst_dst(char ** dst, size_t * cap);
size_t str_add_char(char ** dst, size_t * cap, char c);
ssize_t str_fmt(char ** dst, size_t * capacity, char const * fmt, ...);

// -- Copy --

size_t str_cpy(char ** dst, size_t * cap, StrRef ref);
size_t str_cpy_tolower(char ** dst, size_t * cap, StrRef ref);
size_t str_cpy_toupper(char ** dst, size_t * cap, StrRef ref);

// -- URI encoding --

//size_t str_enc_uri(char ** dst, size_t * cap, StrRef ref);

// -- WWW form encoding --

size_t str_enc_www_form_component_size(MemRef ref);
size_t str_enc_www_form_component(char ** dst, size_t * cap, MemRef ref);

size_t str_enc_www_form_size(StrKeyValRef const * data, size_t cnt);
size_t str_enc_www_form(char ** dst, size_t * cap, StrKeyValRef const * data, size_t cnt);

// -- Data URI encoding --

//size_t str_enc_data_uri_size(StrRef mime, StrRef data);
//size_t str_enc_data_uri(char ** dst, size_t * cap, StrRef mime, StrRef data);

#ifdef __cplusplus
}
#endif
#endif//LIBSTR_FORMAT_H_INCLUDED
