#include "archivist/list.h"
#include "archivist/record.h"
#include "archivist/uuid.h"

arch_record_t *arch_cons_init(arch_record_t *record, arch_uuid_t car, arch_uuid_t cdr, arch_uuid_t ancestor, arch_record_getter_t getter)
{
  record = _arch_record_init(record, ancestor, ARCH_UUID_NIL, getter);
  record->type = ARCH_TYPE_CONS;
  record->width = sizeof(arch_uuid_t);
  record->size = 2;
  ARCH_CAR(record) = car;
  ARCD_CDR(record) = cdr;
  return record;
}
