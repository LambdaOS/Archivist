#include "archivist/hash.h"
#include "archivist/uuid.h"
#include "archivist/record.h"

arch_size_t arch_hash_size(arch_size_t elements)
{
  arch_size_t index = 0;
  for(arch_size_t work = elements >> 1; work; work >>= 1, index++);
  if(elements > (1 << index)) {
    index++;
  }
  return (1 << index);
}

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
