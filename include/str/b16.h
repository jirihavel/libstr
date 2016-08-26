#ifndef LIBSTR_BASE16_H_INCLUDED
#define LIBSTR_BASE16_H_INCLUDED

#include <str/fmt.h>
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

ssize_t str_dec_b16(char ** dst, size_t * cap, StrRef ref);

size_t str_enc_b16(char ** dst, size_t * cap, MemRef ref);
size_t str_enc_B16(char ** dst, size_t * cap, MemRef ref);

#ifdef __cplusplus
}//extern "C" {

namespace str {

template<typename Str>
ssize_t dec_b16(char ** dst, size_t * cap, Str const & str)
{
    return str_dec_b16(dst, cap, str_ref(str));
}

template<typename Mem>
size_t enc_b16(char ** dst, size_t * cap, Mem const & mem)
{
    return str_enc_b16(dst, cap, mem_ref(mem));
}

template<typename Mem>
size_t enc_B16(char ** dst, size_t * cap, Mem const & mem)
{
    return str_enc_B16(dst, cap, mem_ref(mem));
}

}//namespace str

#endif//__cplusplus
#endif//LIBSTR_BASE16_H_INCLUDED
