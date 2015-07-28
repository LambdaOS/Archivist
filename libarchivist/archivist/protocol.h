#ifndef ARCHIVIST_H
#define ARCHIVIST_H 1
#include <stdint.h>
#include "archivist/uuid.h"

typedef uint32_t lflag_t;
#define LF_TRACKING 0x00000001

typedef uint32_t ltype_t;
#define LT_DATUM     1
#define LT_REFERENCE 2
#define LT_CONS      3
#define LT_ARRAY     4
#define LT_TABLE     5

typedef uint64_t lsize_t;

typedef struct {
  luuid_t id;
  ltype_t type;
  lflag_t flags;
  lsize_t revision;
  luuid_t ancestor;
  luuid_t parents_head;  // Head of list of parent objects
  luuid_t refs_head;     // Head of list of tracking references
  lsize_t size;
  uint8_t data[];
} __attribute__((packed)) lrecord_t;
// ^ Eww, compiler dependence. Guess it had to happen somewhere.
#endif
