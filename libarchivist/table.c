#include <stdlib.h>
#include "archivist/record.h"
#include "archivist/table.h"

void _arch_cache_rehash(arch_cache_t *new, arch_cache_bucket_t **slot);

// FNV-1a
arch_hash_t arch_hash_octets(void *datum, size_t count)
{
  uint8_t *octets = (uint8_t *)datum;
  arch_hash_t hash = ARCH_FNV64_OFFSET_BASIS;
  
  for(arch_size_t i = 0; i < count; i++) {
    hash ^= octets[i];
    hash *= ARCH_FNV64_PRIME;
  }

  return hash;
}

arch_hash_t arch_hash_record(arch_record_t *record)
{
  return arch_hash_octets(record->data, (record->size * record->width));
}

arch_hash_t arch_hash_uuid(arch_uuid_t uuid)
{
  return arch_hash_octets(&uuid, sizeof(arch_uuid_t));
}

static arch_cache_bucket_t **_arch_cache_get_slot(arch_cache_t *cache, arch_uuid_t uuid)
{
  return &cache->slots[arch_hash_uuid(uuid) & ARCH_HASH_MASK(cache->size)];
}

void arch_cache_insert(arch_cache_t *cache, arch_cache_bucket_t *bucket)
{
  arch_cache_bucket_t **slot = _arch_cache_get_slot(cache, uuid, ARCH_HASH_MASK(cache->size));

  bucket->next = *slot;
  *slot = bucket;

  cache->entries++;

  return;
}

static void _arch_cache_rehash(arch_cache_t *new, arch_cache_bucket_t **slot)
{
  arch_cache_bucket_t *bucket = *slot;
  *slot = NULL;

  while(bucket) {
    arch_cache_bucket_t *next = bucket->next;
    arch_cache_insert(cache, bucket);
    new->old->entries--;
    bucket = next;
  }
  
  return;
}

arch_record_t *arch_cache_get(arch_cache_t *cache, arch_uuid_t uuid)
{
  arch_cache_bucket_t *bucket = *slot;
  while(bucket) {
    if(bucket->uuid == uuid) {
      return bucket->record;
    }
    bucket = bucket->next;
  }

  if(cache->old) {
    arch_cache_bucket_t **slot = _arch_cache_get_slot(cache->old, uuid);
    bucket = *slot;
    while(bucket) {
      if(bucket->uuid == uuid) {
	_arch_cache_rehash(cache, slot);
	return bucket->record;
      }
      bucket = bucket->next;
    }
  }

  return NULL;
}

