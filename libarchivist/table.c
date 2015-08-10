#include <stdlib.h>
#include "archivist/record.h"
#include "archivist/table.h"

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

arch_record_t *arch_cache_get(arch_cache_t *cache, arch_uuid_t uuid)
{
  size_t hash_mask = ARCH_HASH_MASK(cache->size);

  while (hash_mask > ARCH_CACHE_MIN) {
    arch_cache_bucket_t **current_slot, *current_bucket;
    current_slot = &cache->slots[arch_hash_uuid(uuid) & hash_mask];
    current_bucket = *current_slot;
    while(current_bucket) {
      if(current_bucket->uuid == uuid) {
	arch_record_t *record = current_bucket->record;
	if(hash_mask != ARCH_HASH_MASK(cache->size)){
	  current_bucket = *current_slot;
	  *current_slot = NULL;
	  while(current_bucket) {
	    arch_cache_insert(cache,)
}
	}
	return record;
      } else {
	current_bucket = current_bucket->next;
      }
    }
  }
  
  return NULL;
}

void arch_cache_insert(arch_cache_t *cache, arch_cache_bucket_t *bucket)
{
  arch_cache_bucket_t **slot =
    &cache->slots[arch_hash_uuid(bucket->uuid) & (cache->size - 1)];

  bucket->next = *slot;
  *slot = bucket;

  return;
}
