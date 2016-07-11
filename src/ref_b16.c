#include <str/ref_b16.h>

ssize_t str_decode_b16_size(size_t len);

static inline int B16_dec_char(char x)
{
    x = toupper(x);
    // ... [ 0 ... 9 ] ... [ A ... F ] ...
    switch(x)
    {
        case '0' ... '9' :
            return x - '0';
        case 'A' ... 'F' :
            return x - 'A' + 10;
        default :
            return -1;
    }
}

bool str_decode_b16(char * restrict dst, StrRef ref)
{
    if(ref.len%2 != 0)
    {
        return false;
    }
    char const * const restrict src = ref.ptr;
    for(size_t i = 0; i < ref.len/2; ++i)
    {
        int const a = B16_dec_char(src[2*i]);
        if(a < 0)
        {
            return false;
        }
        int const b = B16_dec_char(src[2*i+1]);
        if(b < 0)
        {
            return false;
        }
        dst[i] = (a<<4) + b;
    }
    dst[ref.len/2] = '\0';
    return true;
}

void str_encode_b16(char * restrict dst, StrRef ref)
{
    static char const HEX[] = "0123456789abcdef";
    char const * const restrict src = ref.ptr;
    for(size_t i = 0; i < ref.len; ++i)
    {
        dst[2*i  ] = HEX[(src[i]>>4)&0xf];
        dst[2*i+1] = HEX[(src[i]   )&0xf];
    }
    dst[2*ref.len] = '\0';
}

void str_encode_B16(char * restrict dst, StrRef ref)
{
    static char const HEX[] = "0123456789ABCDEF";
    char const * const restrict src = ref.ptr;
    for(size_t i = 0; i < ref.len; ++i)
    {
        dst[2*i  ] = HEX[(src[i]>>4)&0xf];
        dst[2*i+1] = HEX[(src[i]   )&0xf];
    }
    dst[2*ref.len] = '\0';
}
