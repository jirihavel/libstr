#ifndef LIBSTR_MEM_H_INCLUDED
#define LIBSTR_MEM_H_INCLUDED

#include <str/api.h>

#ifdef __cplusplus
//# include <cassert>
//# include <climits>
//# include <cstdbool>
# include <cstddef>
extern "C" {
#else
//# include <assert.h>
//# include <limits.h>
//# include <stdbool.h>
# include <stddef.h>
#endif

// -- Interface --

struct MemRef_s;
typedef struct MemRef_s MemRef;

// -- Construction --

inline MemRef mem_ref_null();
inline MemRef mem_ref_empty();
inline MemRef mem_ref(char const * ptr, size_t len);

// -- Queries --

inline bool mem_ref_is_null(MemRef ref);
inline bool mem_ref_is_empty(MemRef ref);

// -- Access --

inline size_t mem_ref_len(MemRef ref);
inline char const * mem_ref_ptr(MemRef ref);

// -- Implementation --

/** \brief Weak reference to memory.
 *
 * It is safely copyable so it can be passed by value.
 */
struct MemRef_s
{
    char const * ptr;
    size_t len;
};

/** \brief Check all invariants.
 *
 * TODO : how to scan it?
 */
#define MEM_REF_ASSERT(ref) do {                        \
    assert((ref).ptr || ((ref).len == 0));              \
    } while(false)

// -- Constructors --

inline MemRef MEM_REF(char const * ptr, size_t len)
{
    return (MemRef) { .ptr = ptr, .len = len };
}

inline MemRef mem_ref(char const * ptr, size_t len)
{
    assert(ptr || (len == 0));
    return MEM_REF(ptr, len);
}

inline MemRef mem_ref_null()
{
    return MEM_REF(NULL, 0);
}

inline MemRef mem_ref_empty()
{
    return MEM_REF("", 0);
}

// -- Queries --

#define MEM_REF_PTR(ref) ((ref).ptr ? (ref).ptr : "")

inline bool mem_ref_is_null(MemRef ref)
{
    MEM_REF_ASSERT(ref);
    return !ref.ptr;
}

inline bool mem_ref_is_empty(MemRef ref)
{
    MEM_REF_ASSERT(ref);
    return ref.len == 0;
}

// -- Access --

inline size_t mem_ref_len(MemRef ref)
{
    MEM_REF_ASSERT(ref);
    return ref.len;
}

inline char const * mem_ref_ptr(MemRef ref)
{
    MEM_REF_ASSERT(ref);
    return MEM_REF_PTR(ref);
}

#ifdef __cplusplus
}//extern "C"

inline MemRef mem_ref(MemRef const & ref) noexcept
{
    MEM_REF_ASSERT(ref);
    return ref;
}

#endif
#endif//LIBSTR_MEM_H_INCLUDED
