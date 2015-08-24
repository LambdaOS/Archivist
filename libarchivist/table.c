#include "archivist/table.h"
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "archivist/hash.h"
#include "archivist/uuid.h"
#include "archivist/record.h"

#define _ARCH_TABLE_INHERITP(record) (record->data[0])
#define _ARCH_TABLE_INDEX_NIL ARCH_SIZE_MAX

const arch_record_t arch_table_record_inherit = {
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

arch_record_t *_arch_table_single_level_get(arch_record_t *table, arch_record_t *key, arch_record_getter_t getter);
bool _arch_table_set(arch_record_t *table, arch_record_t *key, arch_record_t *value, arch_record_getter_t getter);

arch_record_t *_arch_table_single_level_get(arch_record_t *table, arch_record_t *key, arch_record_getter_t getter)
{
  for(arch_table_entry_t *current_entry =
	arch_record_elt(table, arch_hash_record(key) & ARCH_HASH_MASK(table->size));
      current_entry;
      current_entry =
	(current_entry->next != _ARCH_TABLE_INDEX_NIL)
	?arch_record_elt(table, current_entry->next)
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
    for(arch_record_t *current_revision = table; current_revision; getter(current_revision->ancestor)) {
      arch_record_t *value = _arch_table_single_level_get(current_revision, key, getter);
      if(value) {
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
    current_lineage = getter(ARCH_CAR(lineages));
    lineages = getter(ARCH_CDR(lineages));
  }
  return (arch_record_t *)&arch_record_nil;
}

bool _arch_table_set(arch_record_t *table, arch_record_t *key, arch_record_t *value, arch_record_getter_t getter)
{
  arch_table_entry_t *slot =
    arch_record_elt(table, arch_hash_record(key) & ARCH_HASH_MASK(table->size));
  if(!ARCH_UUID_IS_NIL(slot->key)) {
    if(!arch_record_eql(key, getter(slot->key))) {
      errno = EEXIST;
      return false;
    };
    while(slot->next != _ARCH_TABLE_INDEX_NIL) {
      slot = arch_record_elt(table, slot->next);
    }
    arch_table_entry_t *collision_slot;
    arch_size_t index;
    for(index = 0; index < table->size; index++) {
      collision_slot = arch_record_elt(table, index);
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

  slot->key = key->id;
  slot->value = value->id;
  slot->next = _ARCH_TABLE_INDEX_NIL;
  return true;
}

arch_record_t *arch_table_create(arch_table_proto_entry_t *entries, bool tracking, arch_record_getter_t getter)
{
  arch_size_t num_entries = 0;
  for(arch_table_proto_entry_t *entry = entries; entry; entry = entry->next, num_entries++);

  arch_record_t *table = NULL;
  arch_size_t table_bytes =
    sizeof(arch_record_t)
    +sizeof(arch_table_entry_t)
    *arch_hash_size(num_entries);
  if(table_bytes > SIZE_MAX || !(table = malloc((size_t)table_bytes))) {
    errno = ENOMEM;
    return NULL;
  }
  memset(table, 0, (size_t)table_bytes);
  table->size = num_entries;
  table->type = ARCH_TYPE_TABLE;
  table->width = sizeof(arch_table_entry_t);
  if(tracking) table->flags = ARCH_FLAG_TRACKING;

  for(arch_table_proto_entry_t *entry = entries; entry; entry = entry->next) {
    if(!_arch_table_set(table, entry->key, entry->value, getter)) {
      free(table);
      return NULL;
    }
  }
  return table;
}
