#pragma once

#include <stdint.h>

void wang_hash_64bits(const void *key, void *hashval);
void wang_inverse_hash_64bits(const void *hashval, void *key);

void murmurhash3_128bits(const void *key, uint32_t numbytes, void *hashval);
void murmurhash3_64bits(const void *key, uint32_t numbytes, void *hashval);
void murmurhash3_32bits(const void *key, uint32_t numbytes, void *hashval);
