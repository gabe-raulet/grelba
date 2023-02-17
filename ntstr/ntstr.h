#pragma once

#include <stddef.h>

typedef struct
{
    uint64_t *words; /* each word encodes up to 32 nucleotides */
    size_t numwords; /* the total number of words */
    int remain;      /* the number of nucleotides encoded in the very last word */
} _nt_str, *ntstr_t;

ntstr_t ntstr_create(char const *s);
int ntstr_free(ntstr_t nts);
size_t ntstr_ntlen(ntstr_t const nts);
int ntstr_get_cstr(ntstr_t const nts, char *cstr);
