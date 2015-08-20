#ifndef ARCH_TABLE_H
#define ARCH_TABLE_H 1
#include "uuid.h"
#include "record.h"

typedef struct arch_table_entry {
  arch_uuid_t key, value;
  arch_size_t next;
} arch_table_entry_t;

arch_record_t *arch_table_get(arch_record_getter_t getter, arch_record_t *key);
#endif
