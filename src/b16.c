#include <str/b16.h>

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

/** \brief Decode HEX encoded string.
 *
 * \param dst must point to an array at least ref.len/2+1 characters long.
 * \param ref must have even size.
 *
 * \return false when decoding failed - odd ref length or nonhex digit encountered.
 */
bool str_decode_b16(char * restrict dst, StrRef ref)
{
    assert(dst);
    STR_REF_ASSERT(ref);
    StrLen const len = STR_REF_LEN(ref);
    if(len%2 != 0)
    {
        return false;
    }
    char const * const restrict src = ref.ptr;
    for(StrLen i = 0; i < len/2; ++i)
    {
        int const a = B16_dec_char(src[2*i]);
        int const b = B16_dec_char(src[2*i+1]);
        if((a < 0) || (b < 0))
        {
            return false;
        }
        dst[i] = (a<<4) + b;
    }
    dst[len/2] = '\0';
    return true;
}

/** \brief HEX encode binary data.
 *
 * \param dst must point to an array (ref.len*2 + 1) characters long.
 */
void str_encode_b16(char * restrict dst, MemRef ref)
{
    assert(dst);
    MEM_REF_ASSERT(ref);
    static char const HEX[] = "0123456789abcdef";
    char const * const restrict src = ref.ptr;
    for(size_t i = 0; i < ref.len; ++i)
    {
        dst[2*i  ] = HEX[(src[i]>>4)&0xf];
        dst[2*i+1] = HEX[(src[i]   )&0xf];
    }
    dst[2*ref.len] = '\0';
}

void str_encode_B16(char * restrict dst, MemRef ref)
{
    assert(dst);
    MEM_REF_ASSERT(ref);
    static char const HEX[] = "0123456789ABCDEF";
    char const * const restrict src = ref.ptr;
    for(size_t i = 0; i < ref.len; ++i)
    {
        dst[2*i  ] = HEX[(src[i]>>4)&0xf];
        dst[2*i+1] = HEX[(src[i]   )&0xf];
    }
    dst[2*ref.len] = '\0';
}

/** \brief Hex decode string.
 *
 * The string contents are not checked, so decoding can fail even if size succeeds.
 */
ssize_t str_dec_b16_n(char ** dst, size_t * cap, StrRef ref)
{
    STR_REF_ASSERT(ref);
    StrLen const len = STR_REF_LEN(ref);
    ssize_t size = len%2 == 0 ? len/2 : -1;
    if((size >= 0) && dst && cap && (*cap > (size_t)size))
    {
        if(str_decode_b16(*dst, ref))
        {
            *dst += size;
            *cap -= size;
            return size;
        }
        else
        {
            size = -1;
        }
    }
    // decoding error or dst too small
    str_rst_dst(dst, cap); 
    return size;
}

/** \brief Hex encode string using lowercase character.
 *
 * \return 2*len
 */
size_t str_enc_b16_n(char ** dst, size_t * cap, MemRef ref)
{
    MEM_REF_ASSERT(ref);
    if(dst && cap && (*cap > 2*ref.len))
    {
        str_encode_b16(*dst, ref);
        *dst += 2*ref.len;
        *cap -= 2*ref.len;
    }
    else
    {
        str_rst_dst(dst, cap);
    }
    return 2*ref.len;
}

/** \brief Hex encode string using uppercase character.
 *
 * \return 2*len
 */
size_t str_enc_B16_n(char ** dst, size_t * cap, MemRef ref)
{
    MEM_REF_ASSERT(ref);
    if(dst && cap && *cap > 2*ref.len)
    {
        str_encode_B16(*dst, ref);
        *dst += 2*ref.len;
        *cap -= 2*ref.len;
    }
    else
    {
        str_rst_dst(dst, cap);
    }
    return 2*ref.len;
}
