#ifndef LIBSTR_BASE16_H_INCLUDED
#define LIBSTR_BASE16_H_INCLUDED

#include <str/api.h>
#include <str/mem.h>
#include <str/ref.h>

#ifdef __cplusplus
extern "C" {
#endif

//inline OptStrLen str_decode_b16_size(StrLen len)
//{
//    return len%2 == 0 ? (ssize_t)(len/2) : -1;
//}

bool str_decode_b16(char * restrict dst, StrRef src);

void str_encode_b16(char * restrict dst, MemRef src);
void str_encode_B16(char * restrict dst, MemRef src);

#ifdef __cplusplus
}
#endif

#endif//LIBSTR_BASE16_H_INCLUDED
