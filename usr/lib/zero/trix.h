/* libzero tricks and techniques collection... */

/* HAZARD: tricky code to follow... =) */

#ifndef __ZERO_TRIX_H__
#define __ZERO_TRIX_H__

/*
 * This file contains tricks I've gathered together from sources such as MIT
 * HAKMEM and the book Hacker's Delight.
 *
 * REFERENCES
 * ----------
 * http://aggregate.org/MAGIC/
 * http://graphics.stanford.edu/~seander/bithacks.html
 * http://www.hackersdelight.org/
 * http://www.inwap.com/pdp10/hbaker/hakmem/hakmem.html
 */

#define ZEROABS 1

#include <limits.h>
#include <stdint.h>
#include <endian.h>
#include <zero/param.h>
//#include <zero/mtx.h>
#include <zero/asm.h>

/* get the lowest 1-bit in a */
#define lo1bit(a)           ((a) & -(a))
/* get n lowest and highest bits of i */
#define lobits(i, n)        ((i) & ((1U << (n)) - 0x01))
#define hibits(i, n)        ((i) & ~((1U << (sizeof(i) * CHAR_BIT - (n))) - 0x01))
/* get n bits starting from index j */
#define getbits(i, j, n)    (lobits((i) >> (j), (n)))
/* set n bits starting from index j to value b */
#define setbits(i, j, n, b) ((i) |= (((b) << (j)) & ~(((1UL << (n)) << (j)) - 0x01)))
#define bitset(p, b)        (((uint8_t *)(p))[(b) >> 3] & (1UL << ((b) & 0x07)))
/* set bit # b in *p */
#if !defined(setbit)
#define setbit(p, b)        (((uint8_t *)(p))[(b) >> 3] |= (1UL << ((b) & 0x07)))
#endif
/* clear bit # b in *p */
#if !defined(clrbit)
#define clrbit(p, b)        (((uint8_t *)(p))[(b) >> 3] &= ~(1UL << ((b) & 0x07)))
#endif
/* m - mask of bits to be taken from b. */
#define mergebits(a, b, m)  ((a) ^ (((a) ^ (b)) & (m)))
/* m - mask of bits to be copied from a. 1 -> copy, 0 -> leave alone. */
#define copybits(a, b, m) (((a) | (m)) | ((b) & ~(m)))

/* FIXME: test min2() and max2() */

#define min(a, b) ((a) <= (b) ? (a) : (b))
#define max(a, b) ((a) >= (b) ? (a) : (b))

#define min2(a, b) ((b) ^ (((a) ^ (b)) & -((a) < (b))))
#define max2(a, b) ((a) ^ (((a) ^ (b)) & -((a) < (b))))

#define sign(x, nb)                                                     \
    ((x) = (x) & ((1U < (nb)) - 1),                                     \
     ((x) ^ (1U << ((nb) - 1))) - (1U << ((nb) - 1)))
#define sign2(x, nb)                                                    \
    (((x) << (CHAR_BIT * sizeof(x) - (nb))) >> (CHAR_BIT * sizeof(x) - (nb)))

#if 0
/* compute minimum and maximum of a and b without branching */
#define min(a, b)                                                       \
    ((b) + (((a) - (b)) & -((a) < (b))))
#define max(a, b)                                                       \
    ((a) - (((a) - (b)) & -((a) < (b))))
#endif

/* compare with power-of-two p2 */
#define gtpow2(u, p2)  /* true if u > p2 */                             \
    ((u) & ~(p2))
#define gtepow2(u, p2) /* true if u >= p2 */                            \
    ((u) & -(p2))
/* swap a and b without a temporary variable */
#define swap(a, b)     ((a) ^= (b), (b) ^= (a), (a) ^= (b))
#define swap32(a, b)                                                    \
    do {                                                                \
        uint32_t _tmp = a;                                              \
                                                                        \
        a = b;                                                          \
        b = _tmp;                                                       \
    } while (0)
#define ptrswap(a, b)                                                   \
    do {                                                                \
        void *_tmp = a;                                                 \
                                                                        \
        a = b;                                                          \
        b = _tmp;                                                       \
    } while (0)
/* compute absolute value of integer without branching; PATENTED in USA :( */
#if (ZEROABS)
#define zeroabs(a)                                                      \
    (((a) ^ (((a) >> (CHAR_BIT * sizeof(a) - 1))))                      \
     - ((a) >> (CHAR_BIT * sizeof(a) - 1)))
static __inline__ int
abs32(int a)
{
    int _tmp1 = a ^ (((a) >> (CHAR_BIT * sizeof(a) - 1)));
    int _tmp2 = a >> (CHAR_BIT * sizeof(a) - 1);
    int _val = _tmp1 - _tmp2;

    return _val;
}
#define _abs(a)         zeroabs(a)
#define _labs(a)        zeroabs(a)
#define _llabs(a)       zeroabs(a)
#else
#define zeroabs(a) ((a) < 0 ? -(a) : (a))
#endif

#if 0
int       abs(int x);
long      labs(long x);
long long llabs(long long x);
#endif

/* true if x is a power of two */
#if !defined(powerof2)
#define powerof2(x)     (!((x) & ((x) - 1)))
#endif
/* align a to boundary of (the power of two) b2. */
//#define align(a, b2)   ((a) & ~((b2) - 1))
//#define align(a, b2)    ((a) & -(b2))
#define modpow2(a, b2)     ((a) & ((b2) - 1))

/* round a up to the next multiple of (the power of two) b2. */
//#define rounduppow2(a, b2) (((a) + ((b2) - 0x01)) & ~((b2) + 0x01))
#define rounduppow2(a, b2) (((a) + ((b2) - 0x01)) & -(b2))
/* round down to the previous multiple of (the power of two) b2 */
#define rounddownpow2(a, b2) ((a) & ~((b2) - 0x01))

#if !defined(roundup)
#if defined(__GNUC__)
#define roundup(a, b)                                                   \
    ((__builtin_constant_p(b) && powerof2(b))                           \
     ? rounduppow2(a, b)                                                \
     : ((((a) + ((b) - 1)) / (b)) * b))
#else
#define roundup(a, b)                                                   \
    ((((a) + ((b) - 1)) / (b)) * (b))
#endif
#endif /* !defined(roundup) */

/* compute the average of a and b without division */
#define uavg(a, b)      (((a) & (b)) + (((a) ^ (b)) >> 1))

#define divceil(a, b)   (((a) + (b) - 1) / (b))
#define divround(a, b)  (((a) + ((b) / 2)) / (b))

#define haszero_2(a)    (~(a))
#define haszero_32(a)   (((a) - 0x01010101) & ~(a) & 0x80808080)

/* count population of 1 bits in u32; store into r */
#define onebits_32(u32, r)                                              \
    ((r) = (u32),                                                       \
     (r) -= ((r) >> 1) & 0x55555555,                                    \
     (r) = (((r) >> 2) & 0x33333333) + ((r) & 0x33333333),              \
     (r) = ((((r) >> 4) + (r)) & 0x0f0f0f0f),                           \
     (r) += ((r) >> 8),                                                 \
     (r) += ((r) >> 16),                                                \
     (r) &= 0x3f)
#define onebits_32b(u32, r)                                             \
    ((r) = (u32),                                                       \
     (r) -= ((r) >> 1) & 0x55555555,                                    \
     (r) = (((r) >> 2) & 0x33333333) + ((r) & 0x33333333),              \
     (r) = (((((r) >> 4) + (r)) & 0x0f0f0f0f) * 0x01010101) >> 24)

/* store parity of byte b into r */
#define bytepar(b, r)                                                   \
    do {                                                                \
        unsigned long __tmp1;                                           \
                                                                        \
        __tmp1 = (b);                                                   \
        __tmp1 ^= (b) >> 4;                                             \
        (r) = (0x6996 >> (__tmp1 & 0x0f)) & 0x01;                       \
    } while (0)
#define bytepar2(b, r)                                                  \
    do {                                                                \
        unsigned long __tmp1;                                           \
        unsigned long __tmp2;                                           \
                                                                        \
        __tmp1 = __tmp2 = (b);                                          \
        __tmp2 >>= 4;                                                   \
        __tmp1 ^= __tmp2;                                               \
        __tmp2 = 0x6996;                                                \
        (r) = (__tmp2 >> (__tmp1 & 0x0f)) & 0x01;                       \
    } while (0)
#define bytepar3(b) ((0x6996 >> (((b) ^ ((b) >> 4)) & 0x0f)) & 0x01)

#if !defined(__GNUC__)

/* count number of trailing zero-bits in u32 */
#define tzero32(u32, r)                                                 \
    do {                                                                \
        uint32_t __tmp;                                                 \
        uint32_t __mask;                                                \
                                                                        \
        if (u32 == 0) {                                                 \
            (r) = 32;                                                   \
        } else {                                                        \
            (r) = 0;                                                    \
            __tmp = (u32);                                              \
            __mask = 0x01;                                              \
            if (!(__tmp & __mask)) {                                    \
                __mask = 0xffff;                                        \
                if (!(__tmp & __mask)) {                                \
                    __tmp >>= 16;                                       \
                    (r) += 16;                                          \
                }                                                       \
                __mask >>= 8;                                           \
                if (!(__tmp & __mask)) {                                \
                    __tmp >>= 8;                                        \
                    (r) += 8;                                           \
                }                                                       \
                __mask >>= 4;                                           \
                if (!(__tmp & __mask)) {                                \
                    __tmp >>= 4;                                        \
                    (r) += 4;                                           \
                }                                                       \
                __mask >>= 2;                                           \
                if (!(__tmp & __mask)) {                                \
                    __tmp >>= 2;                                        \
                    (r) += 2;                                           \
                }                                                       \
                __mask >>= 1;                                           \
                if (!(__tmp & __mask)) {                                \
                    (r) += 1;                                           \
                }                                                       \
            }                                                           \
        }                                                               \
    } while (0)

#define lzero32(u32, r)                                                 \
    do {                                                                \
        uint32_t __tmp;                                                 \
        uint32_t __mask;                                                \
                                                                        \
        if (u32 == 0) {                                                 \
            (r) = 32;                                                   \
        } else {                                                        \
            (r) = 0;                                                    \
            __tmp = (u32);                                              \
            __mask = 0x01;                                              \
            __mask <<= CHAR_BIT * sizeof(uint32_t) - 1;                 \
            if (!(__tmp & __mask)) {                                    \
                __mask = 0xffffffff;                                    \
                __mask <<= 16;                                          \
                if (!(__tmp & __mask)) {                                \
                    __tmp <<= 16;                                       \
                    (r) += 16;                                          \
                }                                                       \
                __mask <<= 8;                                           \
                if (!(__tmp & __mask)) {                                \
                    __tmp <<= 8;                                        \
                    (r) += 8;                                           \
                }                                                       \
                __mask <<= 4;                                           \
                if (!(__tmp & __mask)) {                                \
                    __tmp <<= 4;                                        \
                    (r) += 4;                                           \
                }                                                       \
                __mask <<= 2;                                           \
                if (!(__tmp & __mask)) {                                \
                    __tmp <<= 2;                                        \
                    (r) += 2;                                           \
                }                                                       \
                __mask <<= 1;                                           \
                if (!(__tmp & __mask)) {                                \
                    (r)++;                                              \
                }                                                       \
            }                                                           \
        }                                                               \
    } while (0)

/* 64-bit versions */

#define tzero64(u64, r)                                                 \
    do {                                                                \
        uint64_t __tmp;                                                 \
        uint64_t __mask;                                                \
                                                                        \
        if (u64 == 0) {                                                 \
            (r) = 64;                                                   \
        } else {                                                        \
            (r) = 0;                                                    \
            __tmp = (u64);                                              \
            __mask = 0x01;                                              \
            if (!(__tmp & __mask)) {                                    \
                __mask = 0xffffffff;                                    \
                if (!(__tmp & __mask)) {                                \
                    __tmp >>= 32;                                       \
                    (r) += 32;                                          \
                }                                                       \
                __mask >>= 16;                                          \
                if (!(__tmp & __mask)) {                                \
                    __tmp >>= 16;                                       \
                    (r) += 16;                                          \
                }                                                       \
                __mask >>= 8;                                           \
                if (!(__tmp & __mask)) {                                \
                    __tmp >>= 8;                                        \
                    (r) += 8;                                           \
                }                                                       \
                __mask >>= 4;                                           \
                if (!(__tmp & __mask)) {                                \
                    __tmp >>= 4;                                        \
                    (r) += 4;                                           \
                }                                                       \
                __mask >>= 2;                                           \
                if (!(__tmp & __mask)) {                                \
                    __tmp >>= 2;                                        \
                    (r) += 2;                                           \
                }                                                       \
                __mask >>= 1;                                           \
                if (!(__tmp & __mask)) {                                \
                    (r) += 1;                                           \
                }                                                       \
            }                                                           \
        }                                                               \
    } while (0)

#define lzero64(u64, r)                                                 \
    do {                                                                \
        uint64_t __tmp;                                                 \
        uint64_t __mask;                                                \
                                                                        \
        if (u64 == 0) {                                                 \
            (r) = 64;                                                   \
        } else {                                                        \
            (r) = 0;                                                    \
            __tmp = (u64);                                              \
            __mask = 0x01;                                              \
            __mask <<= CHAR_BIT * sizeof(uint64_t) - 1;                 \
            if (!(__tmp & __mask)) {                                    \
                __mask = 0xffffffff;                                    \
                __mask <<= 32;                                          \
                if (!(__tmp & __mask)) {                                \
                    __tmp <<= 32;                                       \
                    (r) += 32;                                          \
                }                                                       \
                __mask <<= 16;                                          \
                if (!(__tmp & __mask)) {                                \
                    __tmp <<= 16;                                       \
                    (r) += 16;                                          \
                }                                                       \
                __mask <<= 8;                                           \
                if (!(__tmp & __mask)) {                                \
                    __tmp <<= 8;                                        \
                    (r) += 8;                                           \
                }                                                       \
                __mask <<= 4;                                           \
                if (!(__tmp & __mask)) {                                \
                    __tmp <<= 4;                                        \
                    (r) += 4;                                           \
                }                                                       \
                __mask <<= 2;                                           \
                if (!(__tmp & __mask)) {                                \
                    __tmp <<= 2;                                        \
                    (r) += 2;                                           \
                }                                                       \
                __mask <<= 1;                                           \
                if (!(__tmp & __mask)) {                                \
                    (r)++;                                              \
                }                                                       \
            }                                                           \
        }                                                               \
    } while (0)

#endif /* !defined(__GNUC__) */

/* count number of trailing (low) zero-bits in long-word */
#if defined(__GNUC__)
//#define tzero32(u) (__builtin_ctz(u))
#define tzerol(u)     (!(u)                                             \
                       ? (LONGSIZE * CHAR_BIT)                          \
                       : (__builtin_ctzl(u)))
#define tzeroll(u)    (!(u)                                             \
                       ? (LONGLONGSIZE * CHAR_BIT)                      \
                       : (__builtin_ctzll(u)))
#define tzero32(u, r) (!(u)                                             \
                       ? ((r) = 32)                                     \
                       : ((r) = (__builtin_ctz(u))))
#if (LONGSIZE == 4)
#define tzero64(u, r) (!(u)                                             \
                       ? ((r) = 64)                                     \
                       : ((r) = (__builtin_ctzll(u))))
#elif (LONGSIZE == 8)
#define tzero64(u, r) (!(u)                                             \
                       ? ((r) = 64)                                     \
                       : ((r) = (__builtin_ctzl(u))))
#endif
#elif defined(__i386__) || defined(__x86_64__) || defined(__amd64__)
#define tzerol(u)  (!(u)                                                \
                    ? (LONGSIZE * CHAR_BIT)                             \
                    : m_scanlo1bit(u))
#endif

/* count number of leading (high) zero-bits in long-word */
#if defined(__GNUC__)
#define lzerol(u)     (!(u)                                             \
                       ? (LONGSIZE * CHAR_BIT)                          \
                       : (__builtin_clzl(u)))
#define lzeroll(u)    (!(u)                                             \
                       ? (LONGLONGSIZE * CHAR_BIT)                      \
                       : (__builtin_clzll(u)))
#define lzero32(u, r) (!(u)                                             \
                       ? ((r) = 32)                                     \
                       : ((r) = (__builtin_clz(u))))
#if (LONGSIZE == 4)
#define lzero64(u, r) (!(u)                                             \
                       ? ((r) = 64)                                     \
                       : ((r) = (__builtin_clzll(u))))
#elif (LONGSIZE == 8)
#define lzero64(u, r) (!(u)                                             \
                       ? ((r) = 64)                                     \
                       : ((r) = (__builtin_clzl(u))))
#endif
#elif defined(__i386__) || defined(__x86_64__) || defined(__amd64__)
#define lzerol(u) (!(u)                                                 \
                   ? (LONGSIZE * CHAR_BIT)                              \
                   : (LONGSIZE * CHAR_BIT - m_scanhi1bit(u)))
#endif

/*
 * round longword u to next power of two if not power of two
 */
static __inline__ unsigned long
ceilpow2l(unsigned long u)
{
    long          tmp = sizeof(long) * CHAR_BIT - lzerol(u);
    unsigned long ret;

    if (!powerof2(u)) {
        tmp++;
    }
    ret = 1UL << tmp;

    return ret;
}

#define ceilpow2_32(u, r)                                               \
    do {                                                                \
        (r) = (u);                                                      \
                                                                        \
        if (!powerof2(r)) {                                             \
            (r)--;                                                      \
            (r) |= (r) >> 1;                                            \
            (r) |= (r) >> 2;                                            \
            (r) |= (r) >> 4;                                            \
            (r) |= (r) >> 8;                                            \
            (r) |= (r) >> 16;                                           \
            (r)++;                                                      \
        }                                                               \
    } while (0)
#define ceilpow2_64(u, r)                                               \
    do {                                                                \
        (r) = (u);                                                      \
                                                                        \
        if (!powerof2(r)) {                                             \
            (r)--;                                                      \
            (r) |= (r) >> 1;                                            \
            (r) |= (r) >> 2;                                            \
            (r) |= (r) >> 4;                                            \
            (r) |= (r) >> 8;                                            \
            (r) |= (r) >> 16;                                           \
            (r) |= (r) >> 32;                                           \
            (r)++;                                                      \
        }                                                               \
    } while (0)

/*
 * IEEE 32-bit
 * 0..22  - mantissa
 * 23..30 - exponent
 * 31     - sign
 */
union __ieee754f { uint32_t u32; float f; };

#define fgetmant(f)       (((union __ieee754f *)&(f))->u32 & 0x007fffff)
#define fgetexp(f)        ((((union __ieee754f *)&(f))->u32 & 0x7ff00000) >> 23)
#define fgetsign(f)       (((union __ieee754f *)&(f))->u32 & 0x80000000)
#define fsetmant(f, mant) (((union __ieee754f *)&(f))->u32 |= (mant) & 0x7fffff)
#define fsetexp(f, exp)   (((union __ieee754f *)&(f))->u32 |= (exp) << 23)
#define fsetsign(f, sign)                                               \
    ((sign)                                                             \
     ? (((union __ieee754f *)&(f))->u32 |= 0x80000000)                  \
     : (((union __ieee754f *)&(f))->u32 &= 0x7fffffff))
#define fsetnan(f)                                                      \
    (*(uint32_t *)&(f) = 0x7fffffffU)
#define fsetsnan(f)                                                     \
    (*(uint32_t *)&(f) = 0xffffffffU)

/*
 * IEEE 64-bit
 * 0..51  - mantissa
 * 52..62 - exponent
 * 63     - sign
 */
union __ieee754d { uint64_t u64; double d; };

#define dgetmant(d)       (((union __ieee754d *)(&(d)))->u64 & UINT64_C(0x000fffffffffffff))
#define dgetexp(d)        ((((union __ieee754d *)&(d))->u64 & UINT64_C(0x7ff0000000000000)) >> 52)
#define dgetsign(d)       (((union __ieee754d *)(&(d)))->u64 & UINT64_C(0x8000000000000000))
#define dsetmant(d, mant) (((union __ieee754d *)(&(d)))->u64 |= (mant))
#define dsetexp(d, exp)   (((union __ieee754d *)(&(d)))->u64 |= (uint64_t)(exp) << 52)
#define dsetsign(d, sign)                                               \
    ((sign)                                                             \
     ? (((union __ieee754d *)(&(d)))->u64 |= UINT64_C(0x8000000000000000)) \
     : (((union __ieee754d *)(&(d)))->u64 &= UINT64_C(0x7fffffffffffffff)))
#define dsetnan(d)                                                      \
    (*(uint64_t *)(&(d)) = UINT64_C(0x7fffffffffffffff))
#define dsetsnan(d)                                                     \
    (*(uint64_t *)(&(d)) = UINT64_C(0xffffffffffffffff))

/*
 * IEEE 80-bit
 * 0..63  - mantissa
 * 64..78 - exponent
 * 79     - sign
 */
#define ldgetmant(ld)       (*((uint64_t *)&(ld)))
#define ldgetexp(ld)        (*((uint32_t *)&(ld) + 2) & 0x7fff)
#define ldgetsign(ld)       (*((uint32_t *)&(ld) + 3) & 0x8000)
#define ldsetmant(ld, mant) (*((uint64_t *)&(ld)) = (mant))
#define ldsetexp(ld, exp)   (*((uint32_t *)&(ld) + 2) |= (exp) & 0x7fff)
#define ldsetsign(ld, sign)                                             \
    ((sign)                                                             \
     ? (*((uint32_t *)&ld + 3) |= 0x8000)                               \
     : (*((uint32_t *)&ld + 3) &= 0x7fff))
#if (__BYTE_ORDER == __LITTLE_ENDIAN)
#define ldsetnan(ld)                                                    \
    do {                                                                \
        uint32_t *__u32p = (uint32_t *)&(ld);                           \
                                                                        \
        __u32p[0] = 0xffffffffU;                                        \
        __u32p[1] = 0xffffffffU;                                        \
        __u32p[2] = 0x7fffU;                                            \
    } while (0)
#define ldsetsnan(ld)                                                   \
    do {                                                                \
        uint32_t *__u32p = (uint32_t *)&(ld);                           \
                                                                        \
        __u32p[0] = 0xffffffffU;                                        \
        __u32p[1] = 0xffffffffU;                                        \
        __u32p[2] = 0xffffU;                                            \
    } while (0)
#elif (__BYTE_ORDER == __LITTLE_ENDIAN)
#define ldsetnan(ld)                                                    \
    do {                                                                \
        uint32_t *__u32p = (uint32_t *)&(ld);                           \
                                                                        \
        __u32p[0] = 0x7fffU;                                            \
        __u32p[1] = 0xffffffffU;                                        \
        __u32p[2] = 0xffffffffU;                                        \
    } while (0)
#define ldsetsnan(ld)                                                   \
    do {                                                                \
        uint32_t *__u32p = (uint32_t *)&(ld);                           \
                                                                        \
        __u32p[0] = 0xffffU;                                            \
        __u32p[1] = 0xffffffffU;                                        \
        __u32p[2] = 0xffffffffU;                                        \
    } while (0)
#endif

#if 0
/* sign bit 0x8000000000000000. */
#define ifabs(d)                                                        \
    (_dtou64(d) & UINT64_C(0x7fffffffffffffff))
#define fabs2(d, t64)                                                   \
    (*((uint64_t *)&(t64)) = ifabs(d))
/* sign bit 0x80000000. */
#define ifabsf(f)                                                       \
    (_ftou32(f) & 0x7fffffff)
#endif /* 0 */

/*
 * TODO: IEEE 128-bit
 * - 0..112   - mantissa
 * - 113..125 - exponent
 * - 127      - sign
 */

/* TODO: test the stuff below. */

/* (a < b) ? v1 : v2; */
#define condltset(a, b, v1, v2)                                         \
    (((((a) - (b)) >> (CHAR_BIT * sizeof(a) - 1)) & ((v1) ^ (v2))) ^ (v2))
/* c - conditional, f - flag, u - word */
#define condflgset(c, f, u) ((u) ^ ((-(u) ^ (u)) & (f)))

#define condsetbits(val, mask, cond)                                    \
    ((val) ^= (-(cond) ^ (val)) & (mask))
#define condsetbits2(val, mask, cond)                                   \
    ((val) = ((val) & ~(mask)) | (-(cond) & (mask)))

#define satu8(x)                                                        \
    ((x) <= 0xff ? (x) : 0xff)
#define satu16(x)                                                       \
    ((x) <= 0xffff ? (x) : 0xffff)
#define satu32(x)                                                       \
    ((x) <= 0xffffffff ? (x) : 0xffffffff)
#if 0
#define satu8(x16)                                                      \
    ((x16) | (!((x16) >> 8) - 1))
#define satu16(x32)                                                     \
    ((x32) | (!((x32) >> 16) - 1))
#define satu32(x64)                                                     \
    ((x64) | (!((x64) >> 32) - 1))
#define sat8b(x)                                                        \
    condset(x, 0xff, x, 0xff)
#endif

#define haszero(a) (~(a))
#if 0
#define haszero_32(a)                                                   \
    (~(((((a) & 0x7f7f7f7f) + 0x7f7f7f7f) | (a)) | 0x7f7f7f7f))
#endif

/* calculate modulus u % 10 */
#if 0
#define modu10(u)                                                       \
    ((u) - ((((u) * 6554U) >> 16) * 10))
#endif

#if 0
static __inline__ unsigned long
divu3(unsigned long x)
{
    unsigned long q;
    unsigned long r;

    q = (x >> 2) + (x >> 4);
    q = q + (q >> 4);
    q = q + (q >> 8);
    q = q + (q >> 16);
    r = x - q * 3;

    return q + ((11 * r) >> 5);
}
#endif

static __inline__ unsigned long
divu3(unsigned long uval)
{
    unsigned long long mul = UINT64_C(0xaaaaaaaaaaaaaaab);
    unsigned long long res = uval;
    unsigned long      cnt = 33;

    res *= mul;
    res >>= cnt;
    uval = (unsigned long)res;

    return uval;
}

static __inline__ unsigned long
divu7(unsigned long x)
{
    unsigned long q;
    unsigned long r;

    q = (x >> 1) + (x >> 4);
    q = q + (q >> 6);
    q = q + (q >> 12) + (q >> 24);
    q = q >> 2;
    r = x - q * 7;

    return q + ((r + 1) >> 3);
}

static __inline__ unsigned long
divu9(unsigned long x)
{
    unsigned long q;
    unsigned long r;

    q = x - (x >> 3);
    q = q + (q >> 6);
    q = q + (q >> 12) + (q >> 24);
    q = q >> 3;
    r = x - q * 9;

    return q + ((r + 7) >> 4);
}

static __inline__ unsigned long
divu10(unsigned long x)
{
    unsigned long q;
    unsigned long r;

    q = (x >> 1) + (x >> 2);
    q = q + (q >> 4);
    q = q + (q >> 8);
    q = q + (q >> 16);
    q = q >> 3;
    r = x - q * 10;

    return q + ((r + 6) >> 4);
}

static __inline__ uint32_t
divu60b(uint32_t x)
{
    uint32_t mul = 0x8889;
    uint32_t res;

    res = ((x * mul) >> 16) >> 5;

    return res;

}

static __inline__ uint32_t
divu100b(uint32_t x)
{
    uint32_t mul = 0x47af;
    uint32_t res;

    res = ((((x * mul) >> 16) + x) >> 1) >> 1;

    return res;
}

static __inline__ unsigned long
divu100(unsigned long x)
{
    unsigned long q;
    unsigned long r;

    q = (x >> 1) + (x >> 3) + (x >> 6) - (x >> 10)
        + (x >> 12) + (x >> 13) - (x >> 16);
    q = q + (q >> 20);
    q = q >> 6;
    r = x - q * 100;

    return q + ((r + 28) >> 7);
}

static __inline__ unsigned long
divu1000(unsigned long x)
{
    unsigned long q;
    unsigned long r;
    unsigned long t;

    t = (x >> 7) + (x >> 8) + (x >> 12);
    q = (x >> 1) + t + (x >> 15) + (t >> 11) + (t >> 14);
    q = q >> 9;
    r = x - q * 1000;

    return q + ((r + 24) >> 10);
}

static __inline__ unsigned long
divu1000000(unsigned long x)
{
    unsigned long long tmp = x;
    unsigned long      res;

    tmp *= UINT64_C(0x431bde83);
    tmp >>= 50;
    res = tmp;

    return res;
}

static __inline__ long
divs10(long x)
{
    long q;
    long r;

    x = x + (x >> 31 & 9);
    q = (x >> 1) + (x >> 2);
    q = q + (q >> 4);
    q = q + (q >> 8);
    q = q + (q >> 16);
    q = q >> 3;
    r = x - q * 10;

    return q + ((r + 6) >> 4);
}

static __inline__ long
divs100(long x) {
    long q;
    long r;

    x = x + (x >> 31 & 99);
    q = (x >> 1) + (x >> 3) + (x >> 6) - (x >> 10) +
        (x >> 12) + (x >> 13) - (x >> 16);
    q = q + (q >> 20);
    q = q >> 6;
    r = x - q * 100;

    return q + ((r + 28) >> 7);
}

static __inline__ long
divs1000(long x) {
    long q;
    long r;
    long t;

    x = x + (x >> 31 & 999);
    t = (x >> 7) + (x >> 8) + (x >> 12);
    q = (x >> 1) + t + (x >> 15) + (t >> 11) + (t >> 14) +
        (x >> 26) + (t >> 21);
    q = q >> 9;
    r = x - q * 1000;

    return q + ((r + 24) >> 10);
}

#define modu7(u)   ((u) - divu7(u) * 7)
#define modu9(u)   ((u) - divu9(u) * 9)
#define modu10(u)  ((u) - divu10(u) * 10)
#define modu100(u) ((u) - divu100(u) * 100)
#define modu400(u) ((u) - (divu100(u) >> 2) * 400)

#define leapyear(u)                                                     \
    (!((u) & 0x03) && ((modu100(u)) || !modu400(u)))
#define leapyear2(u)                                                    \
    (!((u) & 0x03) && ((((u) % 100)) || !((u) % 400)))

#define SWAPTMP 1

/* Thanks to Jeremy 'jercos' Sturdivant for this one. */
static __inline__
uint32_t gcdu32(uint32_t a, uint32_t b)
{
    while (b) {
        a %= b;
#if (SWAPTMP)
        swap32(b, a);
#else
        swap(b, a);
#endif
    }

    return a;
}

static __inline__
uint32_t rem32(uint32_t a, uint32_t b)
{
    while (a > b) {
        a -= b;
    }

    return a;
}

/* Thanks to Jeremy 'jercos' Sturdivant for this one. */
static __inline__ void
ratreduce(int64_t *num, int64_t *den)
{
    int64_t a = *num;
    int64_t b = *den;
    int64_t c = a % b;

    while (c) {
        a = b;
        b = c;
        c = a % b;
    }
    *num /= b;
    *den /= b;
}

/* FIXME: use X86 popcnt-instruction for population counts */

/*
 * These were found at http://homepage.cs.uiowa.edu/~jones/bcd/mod.shtml
 */

static __inline__ uint32_t
bitcnt1u32a(uint32_t a) {
    a = ((a >> 1) & 0x55555555) + (a & 0x55555555);
    /* each 2-bit chunk sums 2 bits */
    a = ((a >> 2) & 0x33333333) + (a & 0x33333333);
    /* each 4-bit chunk sums 4 bits */
    a = ((a >> 4) & 0x0F0F0F0F) + (a & 0x0F0F0F0F);
    /* each 8-bit chunk sums 8 bits */
    a = ((a >> 8) & 0x00FF00FF) + (a & 0x00FF00FF);
    /* each 16-bit chunk sums 16 bits */

    return (a >> 16) + (a & 0x0000FFFF);
}

static __inline__ uint32_t
bitcnt1u32(uint32_t a) {
    a = ((a >> 1) & 0x55555555) + (a & 0x55555555);
    a = ((a >> 2) & 0x33333333) + (a & 0x33333333);
    a = ((a >> 4) & 0x07070707) + (a & 0x07070707);
    a = ((a >> 8) & 0x000f000f) + (a & 0x000f000f);

    return (a >> 16) + (a & 0x0000001f);
}

/*
 * this one comes from
 * https://blogs.oracle.com/d/entry/bit_manipulation_population_count
 * - thanks Darryl Gove! :)
 */

static __inline__ uint64_t
bitcnt1u64(uint64_t a)
{
    uint64_t val;
    uint64_t m1 = UINT64_C(0x5555555555555555);
    uint64_t m2 = UINT64_C(0x3333333333333333);
    uint64_t m3 = UINT64_C(0x0f0f0f0f0f0f0f0f);
    uint64_t m4 = UINT64_C(0x00ff00ff00ff00ff);
    uint64_t m5 = UINT64_C(0x0000ffff0000ffff);

    val = a << 1;
    val &= m1;
    a &= m1;
    a += val;
    val = a << 2;
    val &= m2;
    a &= m2;
    a += val;
    val = a << 4;
    val &= m3;
    a &= m3;
    a += val;
    val = a << 8;
    val &= m4;
    a &= m4;
    a += val;
    val = a << 16;
    val &= m5;
    a &= m5;
    a += val;
    val = a << 32;
    a += val;
    a >>= 32;

    return a;
}

/*
 * these bitcnt-routines are from http://bisqwit.iki.fi/source/misc/bitcounting/
 */

static __inline__ uint32_t
bitcnt1u32mul(uint32_t a)
{
    /* dX == (~0) / X */
    uint32_t d3 = 0x55555555;
    uint32_t d5 = 0x33333333;
    uint32_t d17 = 0x0f0f0f0f;
    uint32_t d255 = 0x01010101;

    a -= (a >> 1) & d3;
    a = (a & d5) + ((a >> 2) & d5);
    a = (a + (a >> 4)) & d17;
    a = (a * d255) >> 24;

    return a;
}

static __inline__ uint32_t
bitcnt1u32nomul(uint32_t a)
{
    /* dX == (~0) / X */
    uint32_t d3 = 0x55555555;
    uint32_t d5 = 0x33333333;
    uint32_t d17 = 0x0f0f0f0f;

    a -= (a >> 1) & d3;
    a = (a & d5) + ((a >> 2) & d5);
    a = (a + (a >> 4)) & d17;
    a += a >> 8;
    a += a >> 16;
    a &= 0x7f;

    return a;
}

static __inline__ uint32_t
bitcnt1u64mul(uint32_t a)
{
    /* dX == (~0) / X */
    uint64_t d3 = 0x5555555555555555;
    uint64_t d5 = 0x3333333333333333;
    uint64_t d17 = 0x0f0f0f0f0f0f0f0f;
    uint64_t d255 = 0x0101010101010101;

    a -= (a >> 1) & d3;
    a = (a & d5) + ((a >> 2) & d5);
    a = (a + (a >> 4)) & d17;
    a = (a * d255) >> 56;

    return a;
}

static __inline__ uint32_t
bitcnt1u64nomul(uint64_t a)
{
    /* dX == (~0) / X */
    uint64_t d3 = 0x5555555555555555;
    uint64_t d5 = 0x3333333333333333;
    uint64_t d17 = 0x0f0f0f0f0f0f0f0f;

    a -= (a >> 1) & d3;
    a = (a & d5) + ((a >> 2) & d5);
    a = (a + (a >> 4)) & d17;
    a += a >> 8;
    a += a >> 16;
    a += a >> 32;
    a &= 0x7f;

    return a;
}

static __inline__ uint32_t
mod15u32(uint32_t a)
{
    a = (a >> 16) + (a & 0xffff); /* sum base 2**16 digits */
    a = (a >>  8) + (a & 0xff);   /* sum base 2**8 digits */
    a = (a >>  4) + (a & 0xf);    /* sum base 2**4 digits */
    if (a < 15) return a;
    if (a < (2 * 15)) return a - 15;

    return a - (2 * 15);
}

static __inline__ uint32_t
mod255u32(uint32_t a)
{
    a = (a >> 16) + (a & 0xffff); /* sum base 2**16 digits */
    a = (a >>  8) + (a & 0xff);   /* sum base 2**8 digits */
    if (a < 255) return a;
    if (a < (2 * 255)) return a - 255;

    return a - (2 * 255);
}

static __inline__ uint32_t
mod65535u32(uint32_t a)
{
    a = (a >> 16) + (a & 0xffff); /* sum base 2**16 digits */
    if (a < 65535) return a;
    if (a < (2 * 65535)) return a - 65535;

    return a - (2 * 65535);
}

/*
 * The following routines are implemented as demonstrated at
 *
 * http://locklessinc.com/articles/sat_arithmetic/
 */

/* compute a + b with 32-bit unsigned saturation */
#define sataddu32t(a, b, tmp) ((tmp) = (a) + (b), (tmp) | -((tmp) < (a)))
static __inline__ uint32_t
sataddu32(uint32_t a, uint32_t b)
{
    uint32_t res = a + b;

    res |= -(res < a);  // set res to all 1-bits if overflow occurred

    return res;
}

/* compute a - b with 32-bit unsigned saturation */
#define satsubu32t(a, b, tmp) ((tmp) = (a) - (b), (tmp) & - ((tmp) <= (a)))
static __inline__ uint32_t
satsubu32(uint32_t a, uint32_t b)
{
    uint32_t res = a - b;

    res &= -(res < a);

    return res;
}

/* compute a * b with 32-bit unsigned saturation */
static __inline__ uint32_t
satmulu32(uint32_t a, uint32_t b)
{
    uint64_t a64 = a;
    uint64_t b64 = b;
    uint64_t res = a64 * b64;
    uint32_t hi = res >> 32;
    uint32_t lo = res;
    uint32_t ret = lo | -!!hi;  // set to all 1-bits (-1) if overflow occurred

    return ret;
}

/* compute a / b; no under- or overflow possible */
static __inline__ uint32_t
satdivu32(uint32_t a, uint32_t b)
{
    uint32_t res = a / b;

    return res;
}

/* FIXME: verify chkmulrng32() and chkmulrng64()... :) */

/*
 * check if multiplication will overflow or underflow
 * - return zero if not, negative for underflow, positive for overflow
 */
static __inline__ int32_t
chkmulrng32(int32_t a, int32_t b, int32_t res)
{
    int     nsigbit;    // # of significant bits
    int     tmp;
    int32_t ret;        // return value

    if (!a || !b || a == 1 || b == 1) {
        /* never over- or underflows */

        return 0;
    }
    if (a == INT_MIN || b == INT_MIN) {
        /* always underflows */

        return -1;
    }
    if ((powerof2(a) && res == INT_MIN)
        || (powerof2(b) && res == INT_MIN)) {
        /* okay, minimum negative value */

        return 0;
    }
    a = zeroabs(a);
    b = zeroabs(b);
    lzero32(a, tmp);
    nsigbit = 32 - tmp;
    lzero32(b, tmp);
    nsigbit += 32 - tmp;
    if (nsigbit == 32) {
        /* need slow division */
        ret = (a > (int32_t)(INT_MAX / b));
    } else {
        ret = (nsigbit > 32);
    }

    return ret;
}

/*
 * check if multiplication will overflow or underflow
 * - return zero if not, negative for underflow, positive for overflow
 */
static __inline__ int64_t
chkmulrng64(int64_t a, int64_t b, int64_t res)
{
    int     nsigbit;
    int     tmp;
    int64_t ret;

    if (!a || !b || a == 1 || b == 1) {

        return 0;
    }
    if (a == INT_MIN || b == INT_MIN) {

        return -1;
    }
    if ((powerof2(a) && res == INT_MIN)
        || (powerof2(b) && res == INT_MIN)) {

        return 0;
    }
    a = zeroabs(a);
    b = zeroabs(b);
    lzero64(a, tmp);
    nsigbit = 64 - tmp;
    lzero64(b, tmp);
    nsigbit += 64 - tmp;
    if (nsigbit == 64) {
        ret = (a > INT_MAX / b);
    } else {
        ret = (nsigbit > 64);
    }

    return ret;
}

#endif /* __ZERO_TRIX_H__ */

