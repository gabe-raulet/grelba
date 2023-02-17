#include "hyperloglog.h"
#include "hashfuncs.h"

#include <math.h>
#include <string.h>
#include <stdlib.h>

/* reference: Aydin Buluc modified version of HyperLogLog written by Hideaki Ohno */

#define HASHBITS (64)
#define SIGNBIT (1ULL << (HASHBITS-1))
#define BIG32 ((double)(1ULL << 32))

/* static const double big32_pos = BIG32; */
/* static const double big32_neg = -BIG32; */

struct hyperloglog
{
    uint8_t bits;       /* register bit width */
    uint32_t size;      /* register size */
    double alpha_mm;    /* alpha * m^2 */
    uint8_t *registers; /* registers */
};

static inline uint8_t rho(uint64_t hash, uint8_t bits)
{
    uint8_t v = 1;

    while (v <= bits && !(hash & SIGNBIT))
    {
        v++;
        hash <<= 1;
    }

    return v;
}

hyperloglog_t hyperloglog_init(uint8_t bits)
{
    hyperloglog_t hll = malloc(sizeof(*hll));
    hll->bits = bits;
    hll->size = (1 << bits);
    hll->registers = calloc(hll->size+1, 1);

    double alpha;

    switch (hll->size)
    {
        case 16: alpha = 0.673; break;
        case 32: alpha = 0.697; break;
        case 64: alpha = 0.709; break;
        default: alpha = 0.7213 / (1.0 + (1.079/hll->size));
    }

    hll->alpha_mm = alpha * hll->size * hll->size;

    return hll;
}

void hyperloglog_free(hyperloglog_t hll)
{
    free(hll->registers);
    memset(hll, 0, sizeof(*hll));
    free(hll);
}

void hyperloglog_add(hyperloglog_t hll, char const *s, uint32_t len)
{
    uint64_t hashval;
    murmurhash3_64bits(s, len, &hashval);

    uint32_t index = hashval >> (HASHBITS - hll->bits);
    uint8_t rank = rho((hashval << hll->bits), HASHBITS - hll->bits);

    if (rank > hll->registers[index])
        hll->registers[index] = rank;
}

double hyperloglog_estimate(hyperloglog_t const hll)
{
    double est;
    double sum = 0.0;

    for (uint32_t i = 0; i < hll->size; ++i)
        sum += 1.0 / (1 << hll->registers[i]);

    est = hll->alpha_mm / sum;

    if (est <= 2.5 * hll->size)
    {
        uint32_t zeros = 0;

        for (uint32_t i = 0; i < hll->size; ++i)
            zeros += (hll->registers[i] == 0);

        if (zeros)
        {
            est = hll->size * log((double)hll->size / zeros);
        }
    }

    return est;
}
