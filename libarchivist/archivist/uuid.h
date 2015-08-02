#ifndef ARCH_UUID_H
#define ARCH_UUID_H 1
#include <stdbool.h>
#include <stdint.h>
#include <sys/endian.h>
typedef struct {
  uint64_t low, high;
} __attribute__((packed)) arch_uuid_t;

//Bits 7-6 of VariantAndClockSeqHigh
#define ARCH_UUID_LOW_BITS ((uint64_t)0x2 << 62)
#define ARCH_UUID_VARIANT_MASK ((uint64_t)0x3 << 62)
//Variant 2, as specified by ISO/IEC 9834-8

//Bits 15-11 of VersionAndTimeHigh
#define ARCH_UUID_HIGH_BITS ((uint64_t)0x4 << 12)
#define ARCH_UUID_VERSION_MASK ((uint64_t)0xF << 12)
//Version 4, "random-number-based UUID"

arch_uuid_t arch_uuid_gen(void);

static inline void arch_uuid_enc(void *pp, arch_uuid_t uuid)
{
  uint8_t *p = (uint8_t *)pp;
  le64enc(p, uuid.low);
  le64enc(p + 8, uuid.high);

  return;
}

static inline arch_uuid_t arch_uuid_dec(const void *pp)
{
  uint8_t *p = (uint8_t *)pp;
  return ((arch_uuid_t){ le64dec(p), le64dec(p + 8) });
}

static inline bool arch_uuid_eq(arch_uuid_t uuid1, arch_uuid_t uuid2)
{
  return uuid1.high == uuid2.high?uuid1.low == uuid2.low:false;
}

static inline bool arch_uuid_valid(arch_uuid_t uuid)
{
  return (((uuid.low & ARCH_UUID_VARIANT_MASK) == ARCH_UUID_LOW_BITS) &&
	  ((uuid.high & ARCH_UUID_VERSION_MASK) == ARCH_UUID_HIGH_BITS));
}
#endif
