#include "archivist/list.h"
#include <stdlib.h>
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

arch_list_iterator_t *arch_list_iterator(arch_record_t *head, arch_record_getter_t getter)
{
  if(!head || !getter) {
    return NULL;
  }

  arch_list_iterator_t *iterator = malloc(sizeof(arch_list_iterator_t));
  if(iterator) {
    iterator->head = iterator->current = head;
    iterator->getter = getter;
  }
  return iterator;
}

arch_record_t *arch_list_iterate(arch_list_iterator_t *iterator)
{
  arch_record_t *current = iterator->current;
  if(ARCH_IS(CONS, current)) {
    iterator->current = iterator->getter(CDR(current));
    return getter(CAR(current));
  }
  if(ARCH_IS(NIL, current)) {
    iterator->current = iterator->head;
    return current;
  }
  return NULL;
}
