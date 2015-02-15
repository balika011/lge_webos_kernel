#ifndef _USB3_TYPES_H
#define _USB3_TYPES_H

/*
 * __xx is ok: it doesn't pollute the POSIX namespace. Use these in the
 * header files exported to user space
 */

#define __iomem

typedef enum _bool{false,true} bool;

typedef __signed__ char __s8;
typedef unsigned char __u8;

typedef __signed__ short __s16;
typedef unsigned short __u16;

typedef __signed__ long __s32;
typedef unsigned long __u32;

#if defined(__GNUC__)
__extension__ typedef __signed__ long long __s64;
__extension__ typedef unsigned long long __u64;
#else
typedef unsigned long long  __u64;
typedef signed long long  __s64;
#endif

/*
 * These aren't exported outside the kernel to avoid name space clashes
 */
typedef signed char s8;
typedef unsigned char u8;

typedef signed short s16;
typedef unsigned short u16;

typedef signed long s32;
typedef unsigned long u32;

typedef signed long long s64;
typedef unsigned long long u64;

/* bsd */
typedef unsigned char		u_char;
typedef unsigned short		u_short;
typedef unsigned int		u_int;
typedef unsigned long		u_long;

/* sysv */
typedef unsigned char		unchar;
typedef unsigned short		ushort;
typedef unsigned int		uint;
typedef unsigned long		ulong;

typedef unsigned char		uchar;

typedef u16 __le16;
typedef u16 __be16;
typedef u32 __le32;
typedef u32 __be32;
typedef u64 __le64;
typedef u64 __be64;


#ifdef CFG_64BIT_LBA
typedef uint64_t lbaint_t;
#else
typedef ulong lbaint_t;
#endif

#define ___swab32(x) \
	((__u32)( \
		(((__u32)(x) & (__u32)0x000000ffUL) << 24) | \
		(((__u32)(x) & (__u32)0x0000ff00UL) <<  8) | \
		(((__u32)(x) & (__u32)0x00ff0000UL) >>  8) | \
		(((__u32)(x) & (__u32)0xff000000UL) >> 24) ))

#define cpu_to_le16(x) ((__u16)(x))
#define le16_to_cpu(x) ((__u16)(x))	
#define cpu_to_le32(x) ((__u32)(x))
#define le32_to_cpu(x) ((__u32)(x))
#define cpu_to_le64(x) ((__u64)(x))
#define le64_to_cpu(x) ((__u64)(x))
#define cpu_to_be32(x) ___swab32((x))

#define readl(_reg)  (*((volatile u32*)(_reg)))
#define writel(_val, _reg)  (*((volatile u32*)(_reg)) = (_val))


/* Dma addresses are 32-bits wide.  */

typedef u32 dma_addr_t;

typedef unsigned long phys_addr_t;
typedef unsigned long phys_size_t;


/**
 * upper_32_bits - return bits 32-63 of a number
 * @n: the number we're accessing
 *
 * A basic shift-right of a 64- or 32-bit quantity.  Use this to suppress
 * the "right shift count >= width of type" warning when that quantity is
 * 32-bits.
 */
#define upper_32_bits(n) ((u32)(((n) >> 16) >> 16))

/**
 * lower_32_bits - return bits 0-31 of a number
 * @n: the number we're accessing
 */
#define lower_32_bits(n) ((u32)(n))

#endif
