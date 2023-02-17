#pragma once

#include <stdint.h>

typedef struct hyperloglog *hyperloglog_t;

hyperloglog_t hyperloglog_init(uint8_t bits);
void hyperloglog_free(hyperloglog_t hll);
void hyperloglog_add(hyperloglog_t hll, char const *s, uint32_t len);
double hyperloglog_estimate(hyperloglog_t const hll);
