#ifndef LIBSTR_API_H_INCLUDED
#define LIBSTR_API_H_INCLUDED

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

#endif//LIBSTR_API_H_INCLUDED
