// When defined, correct implementation of vsnprintf is used on windows
#define _ISOC99_SOURCE
#include <str/vsnprintf.h>

#include <stdio.h>

int str_vsnprintf(char * s, size_t n, char const * f, va_list a)
{
    return vsnprintf(s, n, f, a);
}
