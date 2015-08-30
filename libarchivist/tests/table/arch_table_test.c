/*
 * ©2015 Lambda Project, Quinn Evans <heddwch@lambdaos.org>
 * See COPYING for more information
 */
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <err.h>
#include <sysexits.h>
#include <archivist/uuid.h>
#include <archivist/record.h>
#include <archivist/cache.h>
#include <archivist/hash.h>
#include <archivist/table.h>

#define KEYS 4
#define VALUES 6
#define CACHE_ELTS (KEYS + VALUES)

static arch_cache_t *cache;

arch_uuid_t make_random_int(void);
arch_record_t *record_getter(arch_uuid_t uuid);

arch_uuid_t make_random_int(void)
{
  arch_record_t *random_int;
  if(!(random_int=malloc(sizeof(arch_record_t) + sizeof(long)))) {
    return ARCH_UUID_NIL;
  }
  _arch_record_init(random_int, ARCH_UUID_NIL, ARCH_UUID_NIL, &record_getter);
  random_int->width = sizeof(long);
  random_int->size = 1;
  random_int->type = ARCH_TYPE_DATUM;
  *(long *)arch_record_elt(random_int, 0) = random();

  if(!arch_cache_set(cache, random_int)) {
    return ARCH_UUID_NIL;
  }

  return random_int->id;
}

arch_record_t *record_getter(arch_uuid_t uuid)
{
  return arch_cache_get(cache, uuid);
}

int main(int argc, char *argv[]) {
  arch_uuid_t keys[KEYS], values[VALUES];

  if(!(cache = arch_cache_create(CACHE_ELTS))) {
    err(EX_UNAVAILABLE, "failed to allocate cache");
  }
  printf("Cached:\n");
  printf("Keys:\n");
  for(unsigned int i = 0; i < KEYS; i++) {
    keys[i] = make_random_int();
    if(ARCH_UUID_IS_NIL(keys[i])) {
      err(EX_SOFTWARE, "failed to create key %u", i);
    }
    printf("UUID: %lx%lx Value: %ld\n", keys[i].high, keys[i].low, *(long *)arch_record_elt(record_getter(keys[i]), 0));
  }
  printf("Values:\n");
  for(unsigned int i = 0; i < VALUES; i++) {
    values[i] = make_random_int();
    if(ARCH_UUID_IS_NIL(values[i])) {
      err(EX_SOFTWARE, "failed to create value %u", i);
    }
    printf("UUID: %lx%lx Value: %ld\n", values[i].high, values[i].low, *(long *)arch_record_elt(record_getter(values[i]), 0));
  }

  printf("Retrieved keys:\n");
  for(int i = 0; i < KEYS; i++) {
    arch_record_t *key = arch_cache_get(cache, keys[i]);
    if(ARCH_UUID_IS_NIL(key->id)) {
      err(EX_SOFTWARE, "retrieval of UUID %lx%lx failed", keys[i].high, keys[i].low);
    }
    printf("UUID: %lx%lx Value: %ld\n", key->id.high, key->id.low, *(long *)arch_record_elt(key, 0));
  }

  printf("Retrieved values:\n");
  for(int i = 0; i < VALUES; i++) {
    arch_record_t *value = arch_cache_get(cache, values[i]);
    if(ARCH_UUID_IS_NIL(value->id)) {
      err(EX_SOFTWARE, "retrieval of UUID %lx%lx failed", values[i].high, values[i].low);
    }
    printf("UUID: %lx%lx Value: %ld\n", value->id.high, value->id.low, *(long *)arch_record_elt(value, 0));
  }
  int j = 0;
  arch_table_proto_entry_t *entries = NULL;
  
  for(int i = 0; i < KEYS; i++, j++) {
    arch_table_proto_entry_t *entry;
    if(!(entry = arch_table_proto_entry_create(record_getter(keys[i]), record_getter(values[j]), entries))) {
      err(EX_UNAVAILABLE, "Unable to create table proto-entry");
    }
    printf("Created proto-entry %lx%lx/%ld→%lx%lx\n", entry->key->id.high, entry->key->id.low, *(long *)arch_record_elt(entry->key, 0), entry->value->id.high, entry->value->id.low);
    entries = entry;
  }
  printf("Attempting to create table: ");
  arch_record_t *table = arch_table_create(entries, false, ARCH_UUID_NIL, ARCH_UUID_NIL, &record_getter);
  if(!table) {
    err(EX_SOFTWARE, "failed to create table");
  }
  printf("OK\n");

  entries = arch_table_proto_destroy(entries);
  printf("Freed proto-entries.\n");

  for(int i = 0; i < j; i++) {
    arch_record_t *record = record_getter(keys[i]);
    printf("Getting value for key %lx%lx/%ld: ", record->id.high, record->id.low, *(long *)arch_record_elt(record, 0));
    record = arch_table_get(table, record, &record_getter);
    if(ARCH_UUID_IS_NIL(record->id)) {
      err(EX_SOFTWARE, "Failed to get value");
    }
    printf("%lx%lx\n", record->id.high, record->id.low);
  }

  arch_uuid_t old_table = table->id = arch_uuid_gen();
  printf("Caching current table with ID %lx%lx: ", old_table.high, old_table.low);
  if(!arch_cache_set(cache, table)) {
    err(EX_SOFTWARE, "failed to cache table");
  }
  printf("OK\n");

  printf("Assembling proto-entries for shadowing table\n");
  for(int i = 0; j < VALUES; i++, j++) {
    arch_table_proto_entry_t *entry;
    if(!(entry = arch_table_proto_entry_create(record_getter(keys[i]), record_getter(values[j]), entries))) {
      err(EX_UNAVAILABLE, "Unable to create table proto-entry");
    }
    printf("Created proto-entry %lx%lx/%ld→%lx%lx\n", entry->key->id.high, entry->key->id.low, *(long *)arch_record_elt(entry->key, 0), entry->value->id.high, entry->value->id.low);
    entries = entry;
  }

  printf("Attempting to create table: ");
  table = arch_table_create(entries, false, old_table, ARCH_UUID_NIL, &record_getter);
  if(!table) {
    err(EX_SOFTWARE, "failed to create table");
  }
  printf("OK\n");

  entries = arch_table_proto_destroy(entries);
  printf("Freed proto-entries.\n");

  for(int i = 0; i < KEYS; i++) {
    arch_record_t *record = record_getter(keys[i]);
    printf("Getting value for key %lx%lx/%ld: ", record->id.high, record->id.low, *(long *)arch_record_elt(record, 0));
    record = arch_table_get(table, record, &record_getter);
    if(ARCH_UUID_IS_NIL(record->id)) {
      err(EX_SOFTWARE, "Failed to get value");
    }
    printf("%lx%lx\n", record->id.high, record->id.low);
  }
  
  return 0;
}
