#ifndef HASH_H
#define HASH_H 1
#include <stdint.h>
typedef uint64_t lhash_t;

/*
 * CRC-64-ECMA-182
 * x⁶⁴+x⁶²+x⁵⁷+x⁵⁴+x⁵³+x⁵²+x⁴⁷+x⁴⁶+x⁴⁵+x⁴⁰+x³⁹+x³⁸+x³⁷+x³⁵+x³³+x³²+x³¹+x²⁹+x²⁷+x²⁴+x²³+x²²+x²¹+x¹⁹+x¹⁷+x¹³+x¹²+x¹⁰+x⁹+x⁷+x⁴+x+1
 * LSB first
 */
#define CRC_POLYNOMIAL ((lhash_t)0xC96C5795D7870F42)
lhash_t hash_string(const lrecord_t *string);
#endif
