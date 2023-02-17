#include "ntstr.h"
#include "usage.h"
#include "ntlookup.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>


ntstr_t ntstr_create(char const *s)
{
    size_t len = strlen(s);
    size_t numwords = (len + 31) / 32;
    size_t remain = (numwords * 32) - len;

    if (remain >= 32) err("remain >= 32, which shouldn't happen...");

    uint64_t *words = malloc(numwords * 8);
    uint64_t word;

    size_t w = 0;
    char const *sword = s;

    while (w < numwords)
    {
        word = 0;
        int left = (w != numwords-1? 32 : 32-remain);

        for (int i = 0; i < left; ++i)
        {
            word |= ((uint64_t)NT_LOOKUP_CODE(sword[i]) << (62 - (2*i)));
        }

        words[w++] = word;
        sword += 32;
    }

    ntstr_t nts = malloc(sizeof(*nts));
    nts->words = words;
    nts->numwords = numwords;
    nts->remain = (int)remain;

    return nts;
}

int ntstr_free(ntstr_t nts)
{
    if (!nts) return -1;
    free(nts->words);
    memset(nts, 0, sizeof(*nts));
    free(nts);
    return 0;
}

size_t ntstr_ntlen(ntstr_t const nts)
{
    return (nts->numwords * 32) - nts->remain;
}

int ntstr_get_cstr(ntstr_t const nts, char *cstr)
{
    size_t len = ntstr_ntlen(nts);

    uint64_t *wb = nts->words;

    for (size_t i = 0; i < len; ++i)
    {
        int code = (*wb >> (62 - (2*(i % 32)))) & 3;

        cstr[i] = NT_LOOKUP_CHAR(code);

        if ((i+1) % 32 == 0)
            wb++;
    }

    cstr[len] = (char)0;

    return 0;
}

