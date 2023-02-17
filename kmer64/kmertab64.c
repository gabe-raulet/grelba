
#include "kmer64.h"
#include "kmertab64.h"
#include "nextpow2.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

//typedef struct
//{
//    uint64_t v : 54;   /* k-mer bitstring value (can encode up to and including 27-mers) */
//    uint64_t cnt : 10; /* count field (can encode k-mer counts up to and including 1023) */
//} kmertab64_entry_t;

typedef struct
{
    uint64_t v;
    uint64_t cnt;
} kmertab64_entry_t;

struct kmertab64
{
    int kmerlen;            /* self-explanatory */
    int log2size;           /* 2^log2size is the number of available entries in the table */
    int cnt;                /* number of used entries in the table */
    kmertab64_entry_t *tab; /* the actual entries in the table */
};

#define KMERTAB64_MAX_KMERLEN (31)
#define KMERTAB64_SIZE(table) (1ULL << (table)->log2size)
#define KMERTAB64_MASK(table) (KMERTAB64_SIZE(table)-1)
#define KMERTAB64_KMERLEN(table) ((table)->kmerlen)
#define KMERTAB64_CNT(table) ((table)->cnt)
#define KMERTAB64_GET_ENTRY(table, loc) ((table)->tab + (loc))

kmertab64_t kmertab64_init(size_t cardinality_estimate, int kmerlen)
{
    assert(2 <= kmerlen && kmerlen <= KMERTAB64_MAX_KMERLEN);

    size_t size = cardinality_estimate;
    nextpow2(size);
    int log2size = 0;
    while (!(size&1)) { size >>= 1; ++log2size; }

    kmertab64_t table = malloc(sizeof(*table));
    table->kmerlen = kmerlen;
    table->log2size = log2size;
    table->cnt = 0;

    size = KMERTAB64_SIZE(table);
    table->tab = calloc(size, sizeof(kmertab64_entry_t));

    printf("k-mer table initialized with %lu entries\n", size);

    return table;
}

int kmertab64_free(kmertab64_t table)
{
    free(table->tab);
    memset(table, 0, sizeof(*table));
    free(table);
    return 0;
}

void kmertab64_dump(kmertab64_t const table, FILE *f)
{
    size_t size = KMERTAB64_SIZE(table);

    char *kmer = malloc(table->kmerlen + 1);

    for (size_t i = 0; i < size; ++i)
    {
        kmertab64_entry_t *entry = KMERTAB64_GET_ENTRY(table, i);

        if (entry->cnt == 0)
            continue;

        uint64_t code = entry->v;
        uint8_t const *mem = ((uint8_t const *)&code);
        kmer64decode(mem, kmer, table->kmerlen);

        printf("%s\t%llu\n", kmer, entry->cnt);
    }

    free(kmer);
}

int kmertab64_put_kmer(kmertab64_t table, char const *s)
{
    int k = KMERTAB64_KMERLEN(table);
    uint64_t size = KMERTAB64_SIZE(table);
    uint64_t key = kmer64key(s, k);
    uint64_t hashval = kmer64hash(&key);
    uint64_t i;

    for (i = 0; i < size; ++i)
    {
        uint64_t loc = (hashval + ((i*(i+1))>>1)) & KMERTAB64_MASK(table);
        kmertab64_entry_t *entry = KMERTAB64_GET_ENTRY(table, loc);

        if (entry->cnt == 0)
        {
            entry->cnt = 1;
            entry->v = key;
            table->cnt++;
            return 0;
        }
        else if (entry->v == key)
        {
            entry->cnt++;
            return 0;
        }
    }

    return -1;
}

void kmertab64_stats(kmertab64_t const table, size_t *unique, size_t *distinct, size_t *total, size_t *max_count)
{
    size_t size = KMERTAB64_SIZE(table);
    size_t _unique = 0;
    size_t _distinct = table->cnt;
    size_t _total = 0;
    size_t _max_count = 0;

    for (size_t i = 0; i < size; ++i)
    {
        kmertab64_entry_t *entry = KMERTAB64_GET_ENTRY(table, i);

        if (entry->cnt == 0)
            continue;

        if (entry->cnt == 1)
            _unique++;

        _total += entry->cnt;

        _max_count = _max_count > entry->cnt? _max_count : entry->cnt;
    }

    if (unique) *unique = _unique;
    if (distinct) *distinct = _distinct;
    if (total) *total = _total;
    if (max_count) *max_count = _max_count;
}
