#include <stdlib.h>
#include "archivist/record.h"
#include "archivist/table.h"

// Dynamic resize method based on an idea by nortti of Freenode's #osdev-offtopic
void _arch_cache_rehash(arch_cache_t *cache, arch_cache_bucket_t *bucket);

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

static arch_cache_bucket_t **_arch_cache_get_slot(arch_cache_t *cache, arch_uuid_t uuid, size_t mask)
{
  return &cache->slots[arch_hash_uuid(uuid) & mask];
}

void arch_cache_insert(arch_cache_t *cache, arch_cache_bucket_t *bucket)
{
  arch_cache_bucket_t **slot =
    &cache->slots[arch_hash_uuid(bucket->uuid) & ARCH_HASH_MASK(cache->size)];

  bucket->next = *slot;
  *slot = bucket;

  return;
}

static void _arch_cache_rehash(arch_cache_t *cache, arch_cache_bucket_t *bucket)
{
  if(bucket->next) {
    _arch_cache_rehash(cache, bucket->next);
  }

  arch_cache_insert(cache, bucket);
  
  return;
}

arch_record_t *arch_cache_get(arch_cache_t *cache, arch_uuid_t uuid)
{
  size_t hash_mask = ARCH_HASH_MASK(cache->size);

  while (hash_mask > ARCH_CACHE_MIN) {
    arch_cache_bucket_t **slot = &cache->slots[arch_hash_uuid(uuid) & hash_mask];
    arch_cache_bucket_t *bucket = *slot;
    while(bucket) {
      if(bucket->uuid == uuid) {
	arch_record_t *record = bucket->record;
	if(hash_mask != ARCH_HASH_MASK(cache->size)) {
	  *slot = NULL;
	  _arch_cache_rehash(cache, bucket);
	}
	return record;
      } else {
	current_bucket = current_bucket->next;
      }
    }
  }
  
  return NULL;
}

