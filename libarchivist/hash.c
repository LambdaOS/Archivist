#include "archivist/hash.h"

// djb2 by Dan Bernstein
lhash_t hash_string(const lrecord_t *string)
{
  lhash_t hash = 5381;
  
  for(lsize_t i = 0; i < string->size; i++) {
    hash = ((hash << 5) + hash) + string->data[i];
  }
}

