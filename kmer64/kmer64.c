#include "kmer64.h"
#include "hashfuncs.h"
#include "ntlookup.h"
#include <assert.h>
#include <stddef.h> /* size_t */

#define min(a,b) ((a)<(b)?(a):(b))

uint64_t kmer64key(char const *s, int kmerlen)
{
    assert(kmerlen <= 31);
    uint64_t n = (kmerlen + 3) / 4;
    uint64_t mask = n < 8? (1ULL << (n * 8)) - 1 : 0xffffffffffffffff;
    uint8_t mem[8] = {0};

    for (size_t i = 0; i < kmerlen; i += 4)
    {
        int l = min(4, kmerlen - i);

        for (int j = 0; j < l; ++j)
        {
            mem[i>>2] |= (NT_LOOKUP_CODE(s[i+j]) << (2*(3-j)));
        }
    }

    return (*((uint64_t*)mem)) & mask;
}

uint64_t kmer64hash(void const *kmem)
{
    uint64_t hashval;
    murmurhash3_64bits(kmem, 8, (void*)&hashval);
    return hashval;
}

void kmer64decode(uint8_t const *mem, char *kmer, int kmerlen)
{
    for (size_t i = 0; i < kmerlen; i += 4)
    {
        int l = min(4, kmerlen-i);

        for (int j = 0; j < l; ++j)
        {
            kmer[i+j] = "ACGT"[(mem[i>>2] >> (2*(3-j)))&3];
        }
    }

    kmer[kmerlen] = (char)0;
}
