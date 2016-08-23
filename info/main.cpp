#include <str/mem.h>
#include <str/kvr.h>
#include <str/ref.h>

#include <iostream>

int main()
{
    std::cout << "alignments :\n"
        << "  char_ptr: " << alignof(char*) << '\n'
        << "  unsigned: " << alignof(unsigned) << '\n'
        << "  uint8_t: " << alignof(uint8_t) << '\n'
        << "  uint64_t: " << alignof(uint64_t) << '\n';
    std::cout << "mem:\n"
        << "  size: " << sizeof(MemRef) << '\n'
        << "  alignment: " << alignof(MemRef) << '\n'
        << "  max_len: " << SIZE_MAX << '\n';
    std::cout << "ref:\n"
        << "  size: " << sizeof(StrRef) << '\n'
        << "  alignment: " << alignof(StrRef) << '\n'
        << "  max_len: " << STR_LEN_MAX << '\n';
    std::cout << "kvr:\n"
        << "  size: " << sizeof(StrKeyValRef) << '\n'
        << "  alignment: " << alignof(StrKeyValRef) << '\n'
        << "  max_len: " << STR_LEN_MAX << '\n';
    return EXIT_SUCCESS;
}
