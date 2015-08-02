#ifndef ARCH_TABLE_H
#define ARCH_TABLE_H 1
#include <stdint.h>
typedef uint64_t arch_hash_t;

#define ARCH_FNV64_PRIME 1099511628211
#define ARCH_FNV64_OFFSET_BASIS 14695981039346656037

arch_hash_t arch_hash(const arch_record_t *record);
#endif
