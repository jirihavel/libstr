#ifndef LIBSTR_API_H_INCLUDED
#define LIBSTR_API_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include <assert.h>
#include <limits.h>
#include <stdbool.h>

// Only C++ has default function parameters
#if defined(__cplusplus)
# define STR_DEFAULT(x) = x
#else
# define STR_DEFAULT(x)
#endif

// Only C has restrict, otherwise use nonstandard extension
#if defined(__cplusplus) && !defined(restrict)
# define restrict __restrict__
#endif

/** \brief Integral type for string length.
 *
 * Values are in [0, STR_LEN_MAX]. Both int and unsigned can represent all values of StrLen, but its signedness is intentionally undefined.
 */
typedef int StrLen;

#define STR_LEN_MAX INT_MAX

#define STR_LEN_ASSERT(s) do { assert((s) >= 0); assert((s) <= STR_LEN_MAX); } while(false)

/** \brief How the string is terminated.
 */
typedef enum StrTerm_e
{
    /** Terminating character is not guaranteed to be readable. */
    STR_TERM_NONE = 0,
    /** Terminating character is readable, but it's value is unknown. */
    STR_TERM_READ = 1,
    /** The string is zero terminated. */
    STR_TERM_ZERO = 2
} StrTerm;

#ifdef __cplusplus
}
#endif

#endif//LIBSTR_API_H_INCLUDED
