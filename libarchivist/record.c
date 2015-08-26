#include "archivist/record.h"
#include <string.h>
#include <time.h>

const arch_record_t arch_record_nil = {
  ARCH_UUID_NIL,
  0,
  0,
  ARCH_UUID_NIL,
  ARCH_UUID_NIL,
  ARCH_UUID_NIL,
  1,
  ARCH_TYPE_NIL,
  0,
  1,
  { 0 }
};

arch_record_t *_arch_record_init(arch_record_t *record, arch_uuid_t ancestor, arch_uuid_t parents, arch_record_getter_t getter)
{
  memset(record, 0, sizeof(arch_record_t));
  record->ancestor = ancestor;
  record->parents = parents;
  record->timestamp = (arch_time_t)time(NULL);
  if(!ARCH_UUID_IS_NIL(ancestor)) {
    arch_record_t *ancestor = getter(ancestor);
    if(!ARCH_IS(NIL, ancestor)) {
      record->revision = ancestor->revision + 1;
    }
  }

  return record;
}
