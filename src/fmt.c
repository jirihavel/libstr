#define _ISOC99_SOURCE
#include <str/fmt.h>

#include <str/b16.h>

#include <limits.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>

#ifndef SSIZE_MAX
# define SSIZE_MAX ((ssize_t) (SIZE_MAX / 2))
#endif

/** \defgroup StrFmt Components for string formatting.
 * \ingroup Str
 *
 * Pieces, usable to construct snprintf-like functions.
 *
 * All functions str_<type>_... have the form [s]size_t str_...(char ** dst, size_t * cap, ...)
 * * <type> is abbreviation (wrappers have full word)
 * * dst and cap are pointers so the destination address and capacity can be modified
 * * return value is size_t when the formatting can't fail or ssize_t when it can. In case of failure, -1 is returned.
 *   The function returns the number of characters, it wants to write (not including the terminating null char).
 * * if retval >= *cap, then the buffer is too small. The buffer is zero-terminated if *cap allows it but otherwise the contents are unknown.
 *   Also *cap is set to 0 and *dst to NULL, so further writes can be skipped completely.
 * * cap == NULL or *cap == 0 prevent any output. In such case, dst can be NULL, so the functions can serve as size queries.
 * 
 * Sized strings are prefered to zero terminated.
 * * functions can consume any data (no checks for zeroes)
 * * but results are always zero-terminated (if *cap > 0)
 *
 * The return value is the result size if the output buffer is large enough, not including zero terminator.
 *
 * In case of success
 * * <size>+1 (zero terminator) bytes are written do *dst.
 * * <size> is added to *dst and subtracted from *cap.
 * * <size> is returned.
 *
 * In case of failure :
 * * Preferably produce no output (only zero terminator), otherwise a zero terminated string.
 * * *dst is set to NULL and *cap to 0 - following str_... will only calculate size and any misuse will segfault
 * * correct size is returned (so the buffer can be reallocated)
 *
 * Postconditions : 
 * * either *dst == NULL or **dst == '\0'
 * \{
 */

/** \brief Reset/close destination string.
 * 
 * Ensures, string is zero terminated (if cap allows that).
 * Then it sets *dst to NULL so any further errorneous writes segfault.
 * And sets *cap to 0 so no writes should be attempted anyway.
 *
 * (NULL,NULL) - NOP
 * (_, ->?) - *cap = 0
 * (->?, ->1+) - *dst = NULL
 */
void str_rst_dst(char ** dst, size_t * cap)
{
    // force remaining capacity to 0
    size_t old_cap = 0;
    if(cap)
    {
        old_cap = *cap;
        *cap = 0; 
    }
    // zero terminate if possible
    // force dst to NULL
    if(dst && *dst)
    {
        if(old_cap > 0)
        {
            **dst = '\0';
        }
        *dst = NULL;
    }
}

/** Add one character to a string.
 *
 * \return 1
 */
size_t str_add_char(char ** dst, size_t * cap, char c)
{
    if(dst && cap && (*cap > 1))
    {
        **dst = c;
        ++(*dst);
        --(*cap);
        **dst = '\0';
    }
    else
    {
        str_rst_dst(dst, cap);
    }
    return 1;
}

/** \brief Wrapped snprintf.
 *
 * Formats the string, and updates dst and capacity.
 *
 * \return As snprintf : < 0 on error, or the total string length
 */
ssize_t str_fmt(char ** dst, size_t * cap, char const * fmt, ...)
{
    static_assert(INT_MAX <= SSIZE_MAX, "");
    // snprintf
    va_list args;
    va_start(args, fmt);
    int ret = vsnprintf(dst ? *dst : NULL, cap ? *cap : 0, fmt, args);
    va_end(args);
    // Update dst and cap
    if((ret >= 0) && cap && ((size_t)ret < *cap))
    {
        // success
        *dst += ret;
        *cap -= ret;
        assert(**dst == '\0');
    }
    else
    {
        // formatting failure or result too large
        if(ret < -1)
        {
            ret = -1;
        }
        str_rst_dst(dst, cap);
    }
    return ret;
}

// -- Copy --

size_t str_cpy(char ** dst, size_t * cap, StrRef ref)
{
    STR_REF_ASSERT(ref);
    unsigned const len = STR_REF_LEN(ref);
    if(dst && cap && (*cap > len))
    {
        memcpy(*dst, ref.ptr, len);
        *dst += len;
        *cap -= len;
        **dst = '\0';
    }
    else
    {
        str_rst_dst(dst, cap);
    }
    return len;
}

// -- Copy tolower --

size_t str_cpy_tolower(char ** dst, size_t * cap, StrRef ref)
{
    STR_REF_ASSERT(ref);
    unsigned const len = STR_REF_LEN(ref);
    if(dst && cap && (*cap > len))
    {
        for(unsigned i = 0; i < len; ++i)
            (*dst)[i] = tolower(ref.ptr[i]);
        (*dst)[len] = '\0';
        *dst += len;
        *cap -= len;
    }
    else
    {
        str_rst_dst(dst, cap);
    }
    return len;
}

// -- Copy toupper --

size_t str_cpy_toupper(char ** dst, size_t * cap, StrRef ref)
{
    STR_REF_ASSERT(ref);
    unsigned const len = STR_REF_LEN(ref);
    if(dst && cap && (*cap > len))
    {
        for(unsigned i = 0; i < len; ++i)
            (*dst)[i] = toupper(ref.ptr[i]);
        (*dst)[len] = '\0';
        *dst += len;
        *cap -= len;
    }
    else
    {
        str_rst_dst(dst, cap);
    }
    return len;
}

// -- Base16 --
#if 0
/** \brief Hex decode string.
 *
 * The string contents are not checked, so decoding can fail if size succeeds.
 */
ssize_t str_dec_b16_n(char ** dst, size_t * cap, StrRef ref)
{
    ssize_t size = str_decode_b16_size(ref.len);
    if((size >= 0) && (*cap > (size_t)size))
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
size_t str_enc_b16_n(char ** dst, size_t * cap, StrRef ref)
{
    if(*cap > 2*ref.len)
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
size_t str_enc_B16_n(char ** dst, size_t * cap, StrRef ref)
{
    if(*cap > 2*ref.len)
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
#endif
/** \} */
