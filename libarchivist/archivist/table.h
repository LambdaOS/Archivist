#ifndef TABLE_H
#define TABLE_H 1
#include <stdint.h>
typedef uint64_t arch_hash_t;

arch_hash_t arch_hash(const arch_record_t *record);
#endif
