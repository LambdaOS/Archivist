#ifndef ARCHIVIST_H
#define ARCHIVIST_H 1
#include <stdint.h>
#include <string.h>
#include "uuid.h"

typedef uint8_t arch_flag_t;
#define ARCH_FLAG_TRACKING ((arch_flag_t)0x01U)

typedef uint8_t arch_type_t;
#define ARCH_TYPE_NIL       ((arch_type_t)0U)
#define ARCH_TYPE_DATUM     ((arch_type_t)1U)
#define ARCH_TYPE_REFERENCE ((arch_type_t)2U)
#define ARCH_TYPE_CONS      ((arch_type_t)3U)
#define ARCH_TYPE_TABLE     ((arch_type_t)4U)

typedef uint64_t arch_time_t;
typedef uint64_t arch_size_t;
#define ARCH_SIZE_MAX UINT64_MAX

typedef struct {
  arch_uuid_t id;
  arch_time_t timestamp;
  arch_size_t revision;
  arch_uuid_t ancestor;
  arch_uuid_t parents;    // Head of list of parent objects
  arch_uuid_t refs;       // Head of list of tracking references
  arch_size_t size;
  arch_type_t type;
  arch_flag_t flags;
  uint8_t width;
  uint8_t data[] __attribute__((align(sizeof(arch_size_t))));
} __attribute__((packed)) arch_record_t;
// ^ Eww, compiler dependence. Guess it had to happen somewhere.

#define ARCH_IS(type_name, record) (record->type == ARCH_TYPE_##type_name)
#define ARCH_RECORD_BYTES(record) ((arch_size_t)(sizeof(arch_record_t) + record->size * record->width))

extern const arch_record_t arch_record_nil;

typedef arch_record_t *(*arch_record_getter_t)(arch_uuid_t);

static inline void *arch_record_elt(arch_record_t *record, arch_size_t index)
{
  return (record->data + (record->width * index));
}

// Simplistic non-recursive equality check.
// Also blindly truncates lengths to native size_t. You're welcome.
static inline bool arch_record_eql(arch_record_t *record1, arch_record_t *record2)
{
  return
    ((record1->size == record2->size) && (record2->size == record2->size))
    ?(!memcmp(record1->data,
	      record2->data,
	      (size_t)(record1->width*record1->size)))
    :false;
}

/* This is defined in a header for use by other modules in the library. It is not meant to be a client API */
arch_record_t *_arch_record_init(arch_record_t *record, arch_uuid_t ancestor, arch_uuid_t parents, arch_record_getter_t getter);
#endif
