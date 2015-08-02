#ifndef ARCHIVIST_H
#define ARCHIVIST_H 1
#include <stdint.h>
#include "archivist/uuid.h"

typedef uint8_t lflag_t;
#define LF_TRACKING ((lflag_t)0x01)

typedef uint8_t ltype_t;
#define LT_DATUM     ((ltype_t)1)
#define LT_REFERENCE ((ltype_t)2)
#define LT_CONS      ((ltype_t)3)
#define LT_TABLE     ((ltype_t)4)

typedef uint64_t ltime_t;
typedef uint64_t lsize_t;

typedef struct {
  luuid_t id;
  ltime_t timestamp;
  lsize_t revision;
  luuid_t ancestor;
  luuid_t parents_head;  // Head of list of parent objects
  luuid_t refs_head;     // Head of list of tracking references
  lsize_t size;
  ltype_t type;
  lflag_t flags;
  uint8_t width;
  uint8_t data[];
} __attribute__((packed)) lrecord_t;
// ^ Eww, compiler dependence. Guess it had to happen somewhere.
#endif
