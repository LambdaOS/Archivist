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

arch_record_t *make_random_int(void);
arch_record_t *record_getter(arch_uuid_t uuid);

arch_record_t *make_random_int(void)
{
  arch_record_t *random_int;
  if(!(random_int=malloc(sizeof(arch_record_t) + sizeof(long)))) {
    return NULL;
  }
  memset(random_int, 0, sizeof(arch_record_t));
  random_int->width = sizeof(long);
  random_int->size = 1;
  random_int->type = ARCH_TYPE_DATUM;
  *(long *)arch_record_elt(random_int, 0) = random();

  return random_int;
}

arch_record_t *record_getter(arch_uuid_t uuid)
{
  return arch_cache_get(cache, uuid);
}

int main(int argc, char *argv[]) {
  arch_size_t cache_size = arch_hash_size(CACHE_ELTS);
  arch_uuid_t keys[KEYS], values[VALUES];

  printf("Keys:\n");
  for(int i = 0; i < KEYS; i++) {
    keys[i] = arch_uuid_gen();
    printf("%lx%lx\n", keys[i].high, keys[i].low);
  }
  printf("\nValues:\n");
  for(int i = 0; i < VALUES; i++) {
    values[i] = arch_uuid_gen();
    printf("%lx%lx\n", values[i].high, values[i].low);
  }

  if(!(cache=malloc(ARCH_CACHE_BYTES(cache_size)))) {
    err(EX_UNAVAILABLE, "failed to allocate cache");
  }
  memset(cache, 0, ARCH_CACHE_BYTES(cache_size));
  cache->size = cache_size;
  printf("Cached:\n");
  printf("Keys:\n");
  for(int i = 0; i < KEYS; i++) {
    arch_record_t *key;
    if(!(key = make_random_int()));
    key->id = keys[i];
    if(!arch_cache_set(cache, key)) {
      err(EX_SOFTWARE, "Failed in arch_cache_set()");
    }
    printf("UUID: %lx%lx Value: %ld\n", key->id.high, key->id.low, *(long *)arch_record_elt(key, 0));
  }
  printf("Values:\n");
  for(int i = 0; i < VALUES; i++) {
    arch_record_t *value;
    if(!(value = make_random_int()));
    value->id = values[i];
    if(!arch_cache_set(cache, value)) {
      err(EX_SOFTWARE, "Failed in arch_cache_set()");
    }
    printf("UUID: %lx%lx Value: %ld\n", value->id.high, value->id.low, *(long *)arch_record_elt(value, 0));
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
    if(!(entry = malloc(sizeof(arch_table_proto_entry_t)))) {
      err(EX_UNAVAILABLE, "Unable to allocate table proto-entry");
    }
    entry->key = record_getter(keys[i]);
    entry->value = record_getter(values[j]);
    entry->next = entries;
    entries = entry;
    printf("Created proto-entry %lx%lx/%ld→%lx%lx\n", entry->key->id.high, entry->key->id.low, *(long *)arch_record_elt(entry->key, 0), entry->value->id.high, entry->value->id.low);
  }
  printf("Attempting to create table: ");
  arch_record_t *table = arch_table_create(entries, false, &record_getter);
  if(!table) {
    err(EX_SOFTWARE, "failed to create table");
  }
  printf("OK\n");
  while(entries) {
    arch_table_proto_entry_t *entry = entries;
    entries = entry->next;
    free(entry);
  }
  printf("Freed proto-entries.\n");

  for(int i = 0; i < j; i++) {
    arch_record_t *record = record_getter(keys[i]);
    printf("Getting value for key %lx%lx/%ld: ", record->id.high, record->id.low, *(long *)arch_record_elt(record, 0));
    record = arch_table_get(table, record, &record_getter);
    if(ARCH_UUID_IS_NIL(record->id)) {
      errc(EX_SOFTWARE, ENOENT, "Failed to get value");
    }
    printf("%lx%lx\n", record->id.high, record->id.low);
  }
  
  return 0;
}
