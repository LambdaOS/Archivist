#include "archivist/table.h"
#include <stdbool.h>
#include <stdlib.h>
#include "archivist/uuid.h"
#include "archivist/record.h"

#define _ARCH_TABLE_INHERITP(record) (record->data[0]);
#define _ARCH_TABLE_INDEX_NIL ARCH_SIZE_MAX

arch_uuid_t _arch_table_single_level_get(arch_record_t *table, arch_record_t *key, arch_record_getter_t getter);

arch_uuid_t _arch_table_single_level_get(arch_record_t *table, arch_record_t *key, arch_record_getter_t getter)
{
  for(arch_table_entry_t *current_entry =
	((arch_table_entry_t *)table->data)[arch_hash_record(key) & ARCH_HASH_MASK(table->size)];
      current_entry;
      current_entry =
	(current_entry->next != _ARCH_TABLE_INDEX_NIL)
	?((arch_table_entry_t *)table->data)[current_entry->next]
	:NULL) {
    if(arch_record_eql(key, getter(current_entry->key))) {
      return getter(current_entry->value);
    }
  }
  return NULL;
}

arch_record_t *arch_table_get(arch_record_t *table, arch_record_t *key, arch_record_getter_t getter)
{
  arch_record_t *lineages = getter(table->parents_head);
  arch_record_t *current_lineage = table;
  while(true) {
    for(arch_record_t *current_revision; current_revision; getter(current_revision->ancestor)) {
      if((arch_record_t *value = _arch_table_single_level_get(current_revision, key, getter))) {
	if(ARCH_IS(NIL, value)) {
	  if(_ARCH_TABLE_INHERITP(value)) {
	    break;
	  }
	}
	return value;
      }
    }
    if(ARCH_IS(NIL, lineages)) {
      break;
    }
    current_lineage = getter(CAR(lineages));
    lineages = getter(CDR(lineages));
  }
  return &arch_record_nil;
}
