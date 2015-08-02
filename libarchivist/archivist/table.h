#ifndef TABLE_H
#define TABLE_H 1
#include <stdint.h>
typedef uint64_t lhash_t;

lhash_t lhash(const lrecord_t *string);
#endif
