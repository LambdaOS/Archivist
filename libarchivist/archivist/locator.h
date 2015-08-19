#ifndef ARCH_LOCATOR_H
#define ARCH_LOCATOR_H 1
#include "record.h"
#include "uuid.h"

#define ARCH_LOCATOR_MIN 255
typedef struct {
  arch_uuid_t uuid;
  arch_size_t offset, next;
} __attribute__((packed)) arch_locator_bucket_t;

typedef struct {
  arch_size_t size, entries;
  arch_locator_bucket_t slots[];
} __attribute__((packed)) arch_locator_t;

arch_size_t arch_locator_get(arch_locator_t *locator, arch_uuid_t uuid);
bool arch_locator_set(arch_locator_t *locator, arch_uuid_t uuid, arch_size_t offset);
#endif
