#ifndef ARCH_LIST_H
#define ARCH_LIST_H 1
#include "record.h"
#include "uuid.h"

#define ARCH_CONS_BYTES (sizeof(arch_record_t) + sizeof(arch_uuid_t) * 2)
#define ARCH_CAR(record) (*(arch_uuid_t *)arch_record_elt(record, 0))
#define ARCH_CDR(record) (*(arch_uuid_t *)arch_record_elt(record, 1))

typedef struct {
  arch_record_t *head, *current;
  arch_record_getter_t getter;
} arch_list_iterator_t;

arch_record_t *arch_cons_init(arch_record_t *record, arch_uuid_t car, arch_uuid_t cdr, bool tracking,
			      arch_uuid_t ancestor, arch_record_getter_t getter);
arch_list_iterator_t *arch_list_iterator(arch_record_t *head, arch_record_getter_t getter);
arch_record_t *arch_list_iterate(arch_list_iterator_t *iterator);

#endif
