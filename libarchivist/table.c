#include "archivist/table.h"

// djb2 by Dan Bernstein
arch_hash_t hash_string(const lrecord_t *string)
{
  arch_hash_t hash = 5381;
  
  for(arch_size_t i = 0; i < string->size; i++) {
    hash = ((hash << 5) + hash) + string->data[i];
  }
}

