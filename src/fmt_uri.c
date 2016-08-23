#include <str/fmt.h>

//#include <b64/cencode.h>

#include <assert.h>
#include <ctype.h>
#include <stdbool.h>

// returns false for ' ' since it is detected separately
static inline bool must_www_form_escape(char c)
{
    return !isalnum(c) && (c != ' ') && (c != '*') && (c != '-') && (c != '.') && (c != '_');
}

size_t str_enc_www_form_component_size(MemRef ref)
{
    MEM_REF_ASSERT(ref);
    size_t size = 0;
    for(size_t i = 0; i < ref.len; ++i)
    {
        size += must_www_form_escape(ref.ptr[i]) ? 3 : 1;
    }
    return size;
}

size_t str_enc_www_form_component(char ** out, size_t * cap, MemRef ref)
{
    MEM_REF_ASSERT(ref);
    size_t size = 0;
    if(*cap > 0)
    {
        while(ref.len > 0)
        {
            char const c = *ref.ptr;
            ++ref.ptr;
            --ref.len;
            if(must_www_form_escape(c))
            {
                size += 3;
                if(*cap <= (ref.len+3)) // %XX<len>0 will not fit
                    goto input_too_long;
                static char const HEX[] = "0123456789ABCDEF";
                (*out)[0] = '%';
                (*out)[1] = HEX[(c >> 4 ) & 0xF];
                (*out)[2] = HEX[ c        & 0xF];
                *out += 3;
                *cap -= 3;
            }
            else
            {
                ++size;
                if(*cap <= (ref.len+1)) // c<len>0 will not fit
                    goto input_too_long;
                // ' ' -> '+'
                **out = c == ' ' ? '+' : c;
                ++(*out);
                --(*cap);
            }
        }//while
        assert(*cap > 0);
        // finish string
        **out = '\0';
        return size;
    }//if
input_too_long:
    // measure space for remaining stuff
    size += str_enc_www_form_component_size(ref);
    // mark error condition
    str_rst_dst(out, cap);
    return size;
}

size_t str_enc_www_form_size(StrKeyValRef const * data, size_t cnt)
{
    return str_enc_www_form(NULL, 0, data, cnt);
}

size_t str_enc_www_form(char ** out, size_t * cap, StrKeyValRef const * data, size_t cnt)
{
    size_t size = 0;
    for(size_t i = 0; i < cnt; ++i)
    {
        if(i > 0)
        {
            size += str_add_char(out, cap, '&');
        }
        size += str_enc_www_form_component(out, cap, str_ref_mem(str_kvr_key(data[i])));
        StrRef const val = str_kvr_val(data[i]);
        if(!str_ref_is_empty(val))
        {
            size += str_add_char(out, cap, '=');
            size += str_enc_www_form_component(out, cap, str_ref_mem(val));
        }
    }
    return size;
}

/*
size_t str_enc_b64_n(char ** dst, size_t * cap, char const * src, size_t len)
{
    size_t const size = base64_encoded_size(len);
    if(*cap > size)
    {
        size_t ret = base64_encode(src, len, *dst);
        assert(ret == size);
        *dst += ret;// ret is used to prevent warnings
        *cap -= ret;
        **dst = '\0';
    }
    else
    {
        str_rst_dst(dst, cap);
    }
    return size;
}

// -- Data URI --

static char const DURI_DATA[] = "data:";
static char const DURI_B64[] = ";base64,";

STR_API size_t str_size_data_uri_n(char const * mime, size_t msize, char const * data, size_t dsize)
{
    return sizeof(DURI_DATA)-1 + msize + sizeof(DURI_B64)-1 + base64_encoded_size(dsize);
}

STR_API size_t str_enc_data_uri_n(char ** dst, size_t * cap, char const * mime, size_t msize, char const * data, size_t dsize)
{
    size_t const size = str_size_data_uri_n(mime, msize, data, dsize);
    if(*cap > size)
    {
        size_t acc = str_cpy_n(dst, cap, DURI_DATA, sizeof(DURI_DATA)-1);
        acc += str_cpy_n(dst, cap, mime, msize);
        acc += str_cpy_n(dst, cap, DURI_B64, sizeof(DURI_B64)-1);
        acc += str_enc_b64_n(dst, cap, data, dsize);
        assert(acc == size);
    }
    else
    {
        str_rst_dst(dst, cap);
    }
    return size;
}
*/
