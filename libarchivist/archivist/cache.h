#ifndef ARCH_CACHE_H
#define ARCH_CACHE_H 1
#include "uuid.h"
#include "record.h"

#define ARCH_CACHE_MIN 31
typedef struct arch_cache_bucket {
  arch_uuid_t uuid;
  arch_record_t *record;
  struct arch_cache_bucket *next;
} arch_cache_bucket_t;

typedef struct arch_cache {
  struct arch_cache *old;
  size_t size, entries;
  arch_cache_bucket_t *slots[];
} arch_cache_t;

arch_cache_t *arch_cache_create(arch_size_t elts);
arch_cache_t *arch_cache_grow(arch_cache_t *cache);
arch_record_t *arch_cache_get(arch_cache_t *cache, arch_uuid_t uuid);
bool arch_cache_set(arch_cache_t *cache, arch_record_t *record);
void arch_cache_invalidate(arch_cache_t *cache, arch_record_t *record);
#endif
