#ifndef ARCH_HASH_H
#define ARCH_HASH_H 1
#include <sys/types.h>
#include <stdint.h>
#include "uuid.h"
#include "record.h"

#define ARCH_FNV64_PRIME ((arch_hash_t)1099511628211U)
#define ARCH_FNV64_OFFSET_BASIS ((arch_hash_t)14695981039346656037U)
#define ARCH_HASH_MASK(size) (size - 1)

typedef uint64_t arch_hash_t;

arch_size_t arch_hash_size(arch_size_t elements);
arch_hash_t arch_hash_octets(void *datum, size_t count);
arch_hash_t arch_hash_record(arch_record_t *record);
arch_hash_t arch_hash_uuid(arch_uuid_t uuid);
#endif
