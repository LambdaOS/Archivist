#ifndef ARCH_TABLE_H
#define ARCH_TABLE_H 1
#include <sys/types.h>
#include <stdint.h>
typedef uint64_t arch_hash_t;

typedef struct {
  arch_uuid_t key, value;
} __attribute__((packed)) arch_table_member_t;

#define ARCH_FNV64_PRIME ((arch_hash_t)1099511628211U)
#define ARCH_FNV64_OFFSET_BASIS ((arch_hash_t)14695981039346656037U)

arch_hash_t arch_hash(const arch_record_t *record);
arch_record_t arch_create_table(arch_record_t alist);
#endif
