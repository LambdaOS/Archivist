#ifndef ARCHIVIST_H
#define ARCHIVIST_H 1
#include <stdint.h>
#include "archivist/uuid.h"

typedef uint8_t arch_flag_t;
#define ARCH_FLAG_TRACKING ((arch_flag_t)0x01)

typedef uint8_t arch_type_t;
#define ARCH_TYPE_DATUM     ((arch_type_t)1)
#define ARCH_TYPE_REFERENCE ((arch_type_t)2)
#define ARCH_TYPE_CONS      ((arch_type_t)3)
#define ARCH_TYPE_TABLE     ((arch_type_t)4)

typedef uint64_t arch_time_t;
typedef uint64_t arch_size_t;

typedef struct {
  arch_uuid_t id;
  arch_time_t timestamp;
  arch_size_t revision;
  arch_uuid_t ancestor;
  arch_uuid_t parents_head;  // Head of list of parent objects
  arch_uuid_t refs_head;     // Head of list of tracking references
  arch_size_t size;
  arch_type_t type;
  arch_flag_t flags;
  uint8_t width;
  uint8_t data[];
} __attribute__((packed)) arch_record_t;
// ^ Eww, compiler dependence. Guess it had to happen somewhere.
#endif
