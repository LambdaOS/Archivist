#ifndef ARCH_DB_H
#define ARCH_DB_H 1
#include <archivist/record.h>
#include <archivist/uuid.h>
#include <archivist/table.h>
typedef struct {
  arch_uuid_t uuid;
  arch_size_t offset;
} arch_locator_bucket_t;

typedef struct {
  arch_size_t size, entries;
  arch_locator_bucket_t slots[];
} arch_locator_t;


#endif
