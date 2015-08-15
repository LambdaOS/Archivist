#ifndef ARCH_TABLE_H
#define ARCH_TABLE_H 1
#include <sys/types.h>
#include <stdint.h>
#include "uuid.h"

#define ARCH_FNV64_PRIME ((arch_hash_t)1099511628211U)
#define ARCH_FNV64_OFFSET_BASIS ((arch_hash_t)14695981039346656037U)
#define ARCH_HASH_MASK(size) (size - 1)

typedef uint64_t arch_hash_t;

typedef struct {
  arch_uuid_t key, value;
} __attribute__((packed)) arch_slot_t;

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

arch_hash_t arch_hash_octets(void *datum, size_t count);
arch_hash_t arch_hash_record(arch_record_t *record);
arch_hash_t arch_hash_uuid(arch_uuid_t uuid);
arch_record_t *arch_cache_get(arch_cache_t *cache, arch_uuid_t uuid);
bool arch_cache_set(arch_cache_t *cache, arch_record_t *record);
#endif
