#include <stdlib.h>
#include "archivist/uuid.h"

luuid_t luuid_gen()
{
  luuid_t uuid;

  arc4random_buf(&uuid, sizeof(uuid));
  
  uuid.low &= ~(LUUID_VARIANT_MASK);
  uuid.low |= LUUID_LOW_BITS;
  
  uuid.high &= ~(LUUID_VERSION_MASK);
  uuid.high |= LUUID_HIGH_BITS;

  return uuid;
}
