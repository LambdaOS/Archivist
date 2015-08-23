#ifndef ARCH_TABLE_H
#define ARCH_TABLE_H 1
#include <stdint.h>
#include "uuid.h"
#include "record.h"

typedef struct {
  arch_uuid_t key, value;
  arch_size_t next;
} arch_table_entry_t;

typedef struct arch_table_proto_entry {
  arch_record_t *key, *value;
  struct arch_table_proto_entry *next;
} arch_table_proto_entry_t;

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
arch_record_t *arch_table_create(arch_table_proto_entry_t *entries);
#endif
