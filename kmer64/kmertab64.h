#pragma once

#include <stdio.h>
#include <stddef.h>
#include <stdint.h>

typedef struct kmertab64 *kmertab64_t;

kmertab64_t kmertab64_init(size_t cardinality_estimate, int kmerlen);
int kmertab64_free(kmertab64_t table);
int kmertab64_put_kmer(kmertab64_t table, char const *s);
void kmertab64_stats(kmertab64_t const table, size_t *unique, size_t *distinct, size_t *total, size_t *max_count);
