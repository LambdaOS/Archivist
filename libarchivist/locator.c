#include "archivist/locator.h"
#include <stdlib.h>
#include <string.h>
#include "archivist/uuid.h"
#include "archivist/record.h"
#include "archivist/hash.h"

typedef struct _arch_locator_temp_bucket {
  arch_uuid_t uuid;
  arch_size_t offset;
  struct _arch_locator_temp_bucket *next;
} _arch_locator_temp_bucket_t;

void _arch_locator_rehash(arch_locator_t *locator, arch_size_t index);
_arch_locator_temp_bucket_t *_arch_locator_delete_chain(arch_locator_t *locator, arch_size_t index);

_arch_locator_temp_bucket_t *_arch_locator_delete_chain(arch_locator_t *locator, arch_size_t index)
{
  _arch_locator_temp_bucket_t *bucket;
  if(!index) {
    return NULL;
  }
  if((bucket = malloc(sizeof(_arch_locator_temp_bucket_t)))) {
    bucket->uuid = locator->slots[index].uuid;
    bucket->offset = locator->slots[index].offset;
    bucket->next = _arch_locator_delete_chain(locator, locator->slots[index].next);
  }
  memset(&locator->slots[index], 0, sizeof(arch_locator_bucket_t));

  return bucket;
}

// Dynamic resize method based on an idea by nortti of Freenode's #osdev-offtopic
void _arch_locator_rehash(arch_locator_t *locator, arch_size_t index)
{
  _arch_locator_temp_bucket_t *chain = _arch_locator_delete_chain(locator, index);

  while(chain) {
    _arch_locator_temp_bucket_t *temp;
    arch_locator_set(locator, chain->uuid, chain->offset);
    temp = chain->next;
    free(chain);
    chain = temp;
  }

  return;
}

arch_size_t arch_locator_get(arch_locator_t *locator, arch_uuid_t uuid)
{
  arch_hash_t hash_mask = ARCH_HASH_MASK(locator->size);
  while(hash_mask >= ARCH_LOCATOR_MIN) {
    size_t index = (arch_hash_uuid(uuid) & hash_mask);
    while(index) {
      if(arch_uuid_eq(locator->slots[index].uuid, uuid)) {
	arch_size_t offset = locator->slots[index].offset;
	if(hash_mask != ARCH_HASH_MASK(locator->size)) {
	  _arch_locator_rehash(locator, index);
	}
	return offset;
      } else {
	index = locator->slots[index].next;
      }
    }
    hash_mask >>= 1;
  }

  return 0;
}

bool arch_locator_set(arch_locator_t *locator, arch_uuid_t uuid, arch_size_t offset)
{
  arch_size_t index = (arch_hash_uuid(uuid) & ARCH_HASH_MASK(locator->size));
  if(!ARCH_UUID_IS_NIL(locator->slots[index].uuid)) {
    while(locator->slots[index].next) {
      index = locator->slots[index].next;
    }
    arch_size_t collision_index = 0;
    while(!ARCH_UUID_IS_NIL(locator->slots[collision_index].uuid) && (collision_index < locator->size)) {
      collision_index++;
    }
    if(collision_index == locator->size) {
      return false;
    } else {
      locator->slots[index].next = collision_index;
      index = collision_index;
    }
  }
  locator->slots[index] = (arch_locator_bucket_t){ uuid, offset, 0 };
  locator->entries++;

  return true;
}

