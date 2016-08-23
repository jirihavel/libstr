#include <str/mem.h>

MemRef mem_ref_null();
MemRef mem_ref_empty();
MemRef mem_ref(char const * ptr, size_t len);

// -- Queries --

bool mem_ref_is_null(MemRef ref);
bool mem_ref_is_empty(MemRef ref);

// -- Access --

size_t mem_ref_len(MemRef ref);
char const * mem_ref_ptr(MemRef ref);
