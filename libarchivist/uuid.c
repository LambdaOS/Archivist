#include <stdlib.h>
#include "archivist/uuid.h"

arch_uuid_t arch_uuid_gen()
{
  arch_uuid_t uuid;

  arc4random_buf(&uuid, sizeof(uuid));
  
  uuid.low &= ~(ARCH_UUID_VARIANT_MASK);
  uuid.low |= ARCH_UUID_LOW_BITS;
  
  uuid.high &= ~(ARCH_UUID_VERSION_MASK);
  uuid.high |= ARCH_UUID_HIGH_BITS;

  return uuid;
}
