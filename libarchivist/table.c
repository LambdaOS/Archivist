#include "archivist/table.h"

// FNV-1a
arch_hash_t arch_hash(const lrecord_t *record)
{
  arch_hash_t hash = ARCH_FNV64_OFFSET_BASIS;
  
  for(arch_size_t i = 0; i < (record->size * record->width); i++) {
    hash ^= record->data[i];
    hash *= ARCH_FNV64_PRIME;
  }

  return hash;
}

