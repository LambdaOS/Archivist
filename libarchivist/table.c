#include "archivist/table.h"
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "archivist/hash.h"
#include "archivist/uuid.h"
#include "archivist/record.h"

#define _ARCH_TABLE_INHERITP(record) (record->data[0]);
#define _ARCH_TABLE_INDEX_NIL ARCH_SIZE_MAX

arch_record_t *_arch_table_single_level_get(arch_record_t *table, arch_record_t *key, arch_record_getter_t getter);
arch_size_t _arch_table_size(arch_size_t elements);
bool _arch_table_set(arch_record_t *table, arch_record_t *key, arch_record_t *value);

arch_record_t *_arch_table_single_level_get(arch_record_t *table, arch_record_t *key, arch_record_getter_t getter)
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

arch_size_t _arch_table_size(arch_size_t elements)
{
  arch_size_t index = 0;
  for(arch_size_t work = elements >> 1; work; work >>= 1, index++);
  if(elements > (1 << index)) {
    index++;
  }
  return (1 << index);
}

bool _arch_table_set(arch_record_t *table, arch_record_t *key, arch_record_t *value)
{
  arch_table_entry_t *slot =
    &((arch_table_entry_t *)table->data)[arch_hash_record(key) & HASH_MASK(table->size)];
  if(!ARCH_UUID_IS_NIL(slot->key)) {
    if(!arch_record_eql(key, slot->key)) {
      errno = EEXIST;
      return false;
    };
    while(!ARCH_UUID_IS_NIL(slot->next)) {
      slot = &((arch_table_entry_t *)table->data)[slot->next];
    }
    arch_table_entry_t *collision_slot;
    for(arch_size_t index = 0; index < table->size; index++) {
      collision_slot = &((arch_table_entry_t *)table->data)[index];
      if(ARCH_UUID_IS_NIL(collision_slot->key)) {
	slot->next = index;
	slot = collision_slot;
      }
    }
    if(index == table->size) {
      errno = EDOOFUS;
      return false;
    }
  }

  slot->key = key->uuid;
  slot->value = key->value;
  return true;
}

arch_record_t *arch_table_create(arch_table_proto_entry_t *entries, bool tracking)
{
  arch_size_t num_entries = 0;
  for(arch_table_proto_entry_t *entry = entries; entry; entry = entry->next, num_entries++);

  arch_record_t *table = NULL;
  arch_size_t table_bytes =
    sizeof(arch_record_t)
    +sizeof(arch_table_entry_t)
    *_arch_table_size(num_entries);
  if(table_bytes > SIZE_MAX || !(table = malloc((size_t)table_bytes))) {
    return NULL;
  }
  memset(table, 0, (size_t)table_bytes);
  table->size = num_entries;
  table->type = ARCH_TYPE_TABLE;
  table->width = sizeof(arch_table_entry_t);
  if(tracking) table->flags = ARCH_FLAG_TRACKING;

  for(arch_table_proto_entry_t *entry = entries; entry; entry = entry->next) {
    if(!_arch_table_set(table, entry->key, entry->value)) {
      free(table);
      return NULL;
    }
  }
  return table;
}
