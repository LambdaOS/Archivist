#ifndef ARCH_TABLE_H
#define ARCH_TABLE_H 1
#include <stdint.h>
typedef uint64_t arch_hash_t;

#define ARCH_FNV64_PRIME ((arch_hash_t)1099511628211U)
#define ARCH_FNV64_OFFSET_BASIS ((arch_hash_t)14695981039346656037U)

arch_hash_t arch_hash(const arch_record_t *record);
#endif
