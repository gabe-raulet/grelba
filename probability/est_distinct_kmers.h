#pragma once

#include "hyperloglog.h"
#include <string.h>

double estimate_num_distinct_kmers(char **reads, size_t numreads, int kmerlen)
{
    hyperloglog_t hll = hyperloglog_init(12);

    for (size_t id = 0; id < numreads; ++id)
    {
        char *s = reads[id];
        size_t len = strlen(s);

        if (len < kmerlen) continue;

        for (size_t i = 0; i < len-kmerlen+1; ++i)
        {
            hyperloglog_add(hll, s + i, kmerlen);
        }
    }

    double estimate = hyperloglog_estimate(hll);
    hyperloglog_free(hll);

    return estimate;
}
