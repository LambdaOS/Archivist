#include <errno.h>
#include <stdlib.h>
#include "archivist/record.h"
#include "archivist/table.h"

void _arch_cache_rehash(arch_cache_t *new, arch_cache_bucket_t **slot);
bool _arch_cache_insert(arch_cache_t *cache, arch_cache_bucket_t *bucket);

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

bool _arch_cache_insert(arch_cache_t *cache, arch_cache_bucket_t *bucket)
{
  arch_cache_bucket_t **slot = _arch_cache_get_slot(cache, bucket->uuid, ARCH_HASH_MASK(cache->size));

  if(*slot->uuid == bucket->uuid) {  // This does not check the entire chain!
    errno = EEXIST;                  // This only gives you a little bit of safety against
    return false;                    // inserting the same object twice in a row.
  }
  
  bucket->next = *slot;
  *slot = bucket;

  cache->entries++;

  return true;
}

static void _arch_cache_rehash(arch_cache_t *new, arch_cache_bucket_t **slot)
{
  arch_cache_bucket_t *bucket = *slot;
  *slot = NULL;

  while(bucket) {
    arch_cache_bucket_t *next = bucket->next;
    _arch_cache_insert(cache, bucket);
    new->old->entries--;
    bucket = next;
  }

  if(!new->old->entries) {
    free(new->old);
    new->old = 0;
  }
  
  return;
}

arch_record_t *arch_cache_get(arch_cache_t *cache, arch_uuid_t uuid)
{
  arch_cache_bucket_t *bucket = *_arch_cache_get_slot(cache, uuid);
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

bool arch_cache_set(arch_cache_t *cache, arch_record_t *record)
{
  arch_bucket_t *bucket;

  if(!(bucket = malloc(sizeof(arch_bucket_t)))) {
    return false;
  }
  bucket->uuid = record->uuid;
  bucket->record = record;

  if(!_arch_cache_insert(cache, bucket)) {
    free(bucket);
    return false;
  }
  
  return true;
}
