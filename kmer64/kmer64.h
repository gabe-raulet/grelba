#pragma once

#include <stdint.h>

uint64_t kmer64key(char const *s, int kmerlen);
uint64_t kmer64hash(void const *kmem);
void kmer64decode(uint8_t const *mem, char *kmer, int kmerlen);
