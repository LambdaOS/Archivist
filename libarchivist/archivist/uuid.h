#ifndef LUUID_H
#define LUUID_H 1
#include <stdbool.h>
#include <stdint.h>
#include <sys/endian.h>
typedef struct {
  uint64_t low, high;
} __attribute__((packed)) luuid_t;

//Bits 7-6 of VariantAndClockSeqHigh
#define LUUID_LOW_BITS ((uint64_t)2 << 62)
#define LUUID_VARIANT_MASK ((uint64_t)3 << 62)
//Variant 2, as specified by ISO/IEC 9834-8

//Bits 15-11 of VersionAndTimeHigh
#define LUUID_HIGH_BITS ((uint64_t)4 << 12)
#define LUUID_VERSION_MASK ((uint64_t)F << 12)
//Version 4, "random-number-based UUID"

luuid_t luuid_gen();

static inline void luuid_enc(void *pp, luuid_t uuid)
{
  uint8_t *p = (uint8_t *)pp;
  le64enc(p, (uint64_t *)uuid.low);
  le64enc(p + 8, (uint64_t *)uuid.high);

  return;
}

static inline luuid_t luuid_dec(const void *pp)
{
  uint8_t *p = (uint8_t *)pp;
  return ((luuid_t){ le64dec(p), le64dec(p + 8) });
}

static inline bool luuid_eq(luuid_t uuid1, luuid_t uuid2)
{
  return uuid1.high == uuid2.high?uuid1.low == uuid2.low:false;
}
#endif
