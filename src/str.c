#define _ISOC99_SOURCE
#include <str/str.h>

#include <stdio.h>
#if 0
/** \brief Get content type.
 */
StrTag str_str_get_tag(StrStr const * str);

/** \brief Get string length.
 */
int str_str_get_len(StrStr const * str);

/** \brief Get string capacity.
 */
int str_str_get_cap(StrStr const * str);

/** \brief Get string pointer.
 *
 * \return NULL for null string.
 */
char const * str_str_get_ptr(StrStr const * str);

void str_str_set_tag(StrStr * str, StrTag tag);
void str_str_set_len(StrStr * str, int len);
void str_str_set_tag_len_cap(StrStr * str, StrTag tag, int len, int cap);
void str_str_set_len_cap(StrStr * str, int len, int cap);

bool str_str_is_null(StrStr const * str);
bool str_str_is_empty(StrStr const * str);
bool str_str_is_mutable(StrStr const * str);

int str_str_len(StrStr const * str);
int str_str_cap(StrStr const * str);
char const * str_str_ptr(StrStr const * str);
char * str_str_ptr_mut(StrStr * str);
StrRef str_str_ref(StrStr const * str);

// -- Initialization --

/** \brief Initialize string as null.
 */
void str_str_init_null(StrStr * str);

/** \brief Initialize string as empty.
 */
void str_str_init_empty(StrStr * str);

/** \brief Initialize string by move.
 *
 * Moves contents of src to dst. src will be null.
 */
void str_str_init_move(StrStr * restrict dst, StrStr * restrict src);

/** \brief Initialize string as weak mutable reference.
 *
 * len < 0 - ptr is zero terminated, use strlen
 * cap == 0 - string will be immutable
 * cap > len - string will be mutable and zero terminated
 *
 * char aux[256];
 * StrStr str;
 * str_str_init_weak(&str, aux, 0, 256);
 * str_str_fmt(&str, ...); // reallocates if more than 256B is needed
 */
void str_str_init_weak(StrStr * str, char * ptr, int len, unsigned cap);

/** \brief Initialize as weak constant reference.
 */
void str_str_init_const(StrStr * str, StrRef ref);

// -- Deinitialization --

void str_str_kill(StrStr * str);

// -- Assignment --

void str_str_set_null(StrStr * str);
void str_str_set_empty(StrStr * str);
void str_str_set_weak(StrStr * str, char * ptr, int len, unsigned cap);

// -- Allocation --

bool str_str_alloc(StrStr * str, int cap, int len)
{
    STR_STR_ASSERT(str);
    if(cap < 0) // cap < 0 = guess required size
        cap = str_str_get_len(str);
    if(cap == 0)
    {
        str_str_set_empty(str);
        return true;
    }
    assert(cap > 0);
    if(len > cap) // only new cap bytes can be preserved
        len = cap;
    {
        int old_len = str_str_get_len(str);
        if(len > old_len) // only old len bytes can be preserved
            len = old_len;
    }
    if(cap > str_str_get_cap(str))
    {
        if(cap <= (int)STR_SSO_CAP)
        {
            if(str_str_get_tag(str) != STR_TAG_SSO)
            {
                StrStr old;
                memcpy(&old, str, sizeof(StrStr));
                if(len >= 0)
                    memcpy(str->sso.dat, old.rep.ptr, len);
                str_str_set_tag(str, STR_TAG_SSO);
                str_str_kill(&old);
            }
            if(len >= 0)
            {
                ((char*)str_str_get_ptr(str))[len] = '\0';
                str_str_set_len(str, len);
            }
        }
        else
        {
            char * ptr = malloc(cap+1u);
            if(!ptr)
                return false;
            if(len >= 0)
            {
                memcpy(ptr, str_str_get_ptr(str), len);
                ptr[len] = '\0';
            }
            str_str_kill(str);
            str->rep.ptr = ptr;
            str_str_set_tag_len_cap(str, STR_TAG_STR, len < 0 ? 0 : len, cap);
        }
    }
    assert(str_str_cap(str) >= cap); // PRE str ok
    return true;
}

// -- Modification --

char const * str_str_cstr(StrStr * str);
bool str_str_cat(StrStr * str, StrRef ref);

bool str_str_fmt(StrStr * str, char const * fmt, ...)
{
    char * ptr = str_str_ptr_mut(str);// PRE str ok
    unsigned const cap = ptr ? str_str_cap(str)+1u : 0;
    va_list args;
    // first try
    va_start(args, fmt);
    int const len = vsnprintf(ptr, cap, fmt, args);
    va_end(args);
    bool ok = len >= 0;
    if(ok && ((unsigned)len >= cap)) // string is too small
    {
        str_str_set_len(str, cap>0 ? cap-1 : 0);
        if(str_str_alloc(str, len, len))
        {
            ptr = str_str_ptr_mut(str);
            // retry
            va_start(args, fmt);
            ok = vsnprintf(ptr, len+1u, fmt, args) == len;
            va_end(args);
        }
    }
    if(ok)
        str_str_set_len(str, len);
    else
        str_str_set_null(str);
    return ok;
}
#endif
