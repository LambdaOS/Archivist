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

extern const arch_record_t arch_table_record_inherit;

arch_record_t *arch_table_get(arch_record_t *table, arch_record_t *key, arch_record_getter_t getter);
arch_table_proto_entry_t *arch_table_proto_entry_create(arch_record_t *key, arch_record_t *value,
							arch_table_proto_entry_t *next);
arch_table_proto_entry_t *arch_table_proto_destroy(arch_table_proto_entry_t *entries);
arch_record_t *arch_table_create(arch_table_proto_entry_t *entries, bool tracking,
				 arch_uuid_t ancestor, arch_uuid_t parents, arch_record_getter_t getter);
#endif
