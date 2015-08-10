#include "locator.h"

void _arch_locator_rehash(arch_locator_t *locator, arch_size_t index);

arch_size_t arch_locator_get(arch_locator_t *locator, arch_uuid_t uuid)
{
  arch_hash_t hash_mask = ARCH_HASH_MASK(locator->size);
  while(hash_mask >= ARCH_LOCATOR_MIN) {
    size_t index = (arch_hash_uuid(uuid) & hash_mask);
    while(index) {
      if(locator->slots[index].uuid == uuid) {
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

  return NULL;
}

bool arch_locator_set(arch_locator_t *locator, arch_uuid_t uuid, arch_size_t offset)
