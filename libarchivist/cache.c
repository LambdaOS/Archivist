#include <errno.h>
#include <stdlib.h>
#include "archivist/uuid.h"
#include "archivist/record.h"
#include "archivist/hash.h"
#include "archivist/cache.h"

static bool _arch_cache_insert(arch_cache_t *cache, arch_cache_bucket_t *bucket);
static void _arch_cache_rehash(arch_cache_t *new, arch_cache_bucket_t **slot);

static arch_cache_bucket_t **_arch_cache_get_slot(arch_cache_t *cache, arch_uuid_t uuid)
{
  return &cache->slots[arch_hash_uuid(uuid) & ARCH_HASH_MASK(cache->size)];
}

static bool _arch_cache_insert(arch_cache_t *cache, arch_cache_bucket_t *bucket)
{
  arch_cache_bucket_t **slot = _arch_cache_get_slot(cache, bucket->uuid);

  if(arch_uuid_eq((*slot)->uuid, bucket->uuid)) {  // This does not check the entire chain!
    errno = EEXIST;                              // This only gives you a little bit of safety against
    return false;                                // inserting the same object twice in a row.
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
    _arch_cache_insert(new, bucket);
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
    if(arch_uuid_eq(bucket->uuid, uuid)) {
      return bucket->record;
    }
    bucket = bucket->next;
  }

  if(cache->old) {
    arch_cache_bucket_t **slot = _arch_cache_get_slot(cache->old, uuid);
    bucket = *slot;
    while(bucket) {
      if(arch_uuid_eq(bucket->uuid, uuid)) {
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
  arch_cache_bucket_t *bucket;

  if(!(bucket = malloc(sizeof(arch_cache_bucket_t)))) {
    return false;
  }
  bucket->uuid = record->id;
  bucket->record = record;

  if(!_arch_cache_insert(cache, bucket)) {
    free(bucket);
    return false;
  }
  
  return true;
}
