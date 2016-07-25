#include <str/kvr.h>

StrKeyValRef str_kvr(StrRef key, StrRef val);
StrRef str_kvr_key(StrKeyValRef const * kvr);
StrRef str_kvr_val(StrKeyValRef const * kvr);
