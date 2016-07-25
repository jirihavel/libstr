/*
cencode.h - c header for a base64 encoding algorithm

This is part of the libb64 project, and has been placed in the public domain.
For details, see http://sourceforge.net/projects/libb64

Modified 2014-15 by george@angelcam.com
- size_t
- Removed insertion of line feeds (rfc4648, maybe I return it as optional)
- base64_encoded_size
- base64_encode
*/

#ifndef BASE64_CENCODE_H
#define BASE64_CENCODE_H

#include <stddef.h>

typedef enum
{
	step_A, step_B, step_C
} base64_encodestep;

typedef struct
{
	base64_encodestep step;
	char result;
//	int stepcount;
} base64_encodestate;

inline size_t base64_encoded_size(size_t size)
{
    return (size+2)/3*4;
} 

void base64_init_encodestate(base64_encodestate * state_in);

char base64_encode_value(char value_in);

size_t base64_encode_block(const char * plaintext_in, size_t length_in, char * code_out, base64_encodestate * state_in);

size_t base64_encode_blockend(char * code_out, base64_encodestate * state_in);

size_t base64_encode(char const * in, size_t len, char * out);

#endif /* BASE64_CENCODE_H */
