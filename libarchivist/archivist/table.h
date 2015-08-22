#ifndef ARCH_TABLE_H
#define ARCH_TABLE_H 1
#include <stdint.h>
#include "uuid.h"
#include "record.h"

typedef struct arch_table_entry {
  arch_uuid_t key, value;
  arch_size_t next;
} arch_table_entry_t;

arch_record_t arch_table_record_inherit = {
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
  { 1 }
};

arch_record_t *arch_table_get(arch_record_t *table, arch_record_t *key, arch_record_getter_t getter);
bool arch_table_set(arch_record_t *key, arch_record_t *value);
#endif
