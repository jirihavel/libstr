#ifndef LIBSTR_REF_BASE16_H_INCLUDED
#define LIBSTR_REF_BASE16_H_INCLUDED

#include <str/api.h>
#include <str/ref.h>

#ifdef __cplusplus
extern "C" {
#endif

inline ssize_t str_decode_b16_size(size_t len)
{
    return len%2 == 0 ? (ssize_t)(len/2) : -1;
}

bool str_decode_b16(char * restrict dst, StrRef src);

void str_encode_b16(char * restrict dst, StrRef src);
void str_encode_B16(char * restrict dst, StrRef src);

#ifdef __cplusplus
}
#endif

#endif//LIBSTR_REF_BASE16_H_INCLUDED
