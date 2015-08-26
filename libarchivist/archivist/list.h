#ifndef ARCH_LIST_H
#define ARCH_LIST_H 1
#include "record.h"
#include "uuid.h"

#define ARCH_CAR(record) (*((arch_uuid_t *)arch_record_elt(record, 0)))
#define ARCH_CDR(record) (*((arch_uuid_t *)arch_record_elt(record, 1)))

arch_record_t *arch_cons_init(arch_uuid_t car, arch_uuid_t cdr);
#endif
