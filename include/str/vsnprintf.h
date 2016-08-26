#ifndef LIBSTR_VSNPRINTF_H_INCLUDED
#define LIBSTR_VSNPRINTF_H_INCLUDED

#include <str/api.h>

#ifdef __cplusplus
# include <cstdarg>
# include <cstddef>
extern "C" {
#else
# include <stdarg.h>
# include <stddef.h>
#endif

/** \brief Tiny wrapper over vsnprintf.
 *
 * Does not use buggy windows implementation.
 */
int str_vsnprintf(char * s, size_t n, char const * f, va_list a);

#ifdef __cplusplus
}//extern "C"
#endif
#endif//LIBSTR_VSNPRINTF_H_INCLUDED
