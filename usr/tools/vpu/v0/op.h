#ifndef __V0_OP_H__
#define __V0_OP_H__

/* FIXME: make this file work */

#include <v0/conf.h>
#include <stddef.h>
#include <limits.h>
#include <stdlib.h>
#include <stdint.h>
#include <zero/param.h>
#include <zero/cdefs.h>
#include <zero/trix.h>
#include <zero/fastudiv.h>
#include <v0/mach.h>

#define V0_OP_INVAL             NULL
#define V0_ADR_INVAL            0x00000000
#define V0_CNT_INVAL            (-1)
#if !defined(__GNUC__)
#define _V0OP_T FASTCALL INLINE void *
#else
#define _V0OP_T INLINE          void *
#endif

#if defined(V0_GAME)
static long long                v0speedcnt;
#define v0addspeedcnt(n)        (v0speedcnt += (n))
#else
#define v0addspeedcnt(n)
#endif

#define v0adrtoptr(vm, adr)     ((void *)(&(vm)->mem[(adr)]))
#define v0doxcpt(xcpt)                                                  \
    do {                                                                \
        fprintf(stderr, "%s\n", #xcpt);                                 \
                                                                        \
        exit(xcpt);                                                     \
    } while (0)

/* register or immediate source operand */
#define v0getsrc(vm, op)                                                \
    (((op)->adr == V0_REG_ADR)                                          \
     ? ((vm)->regs.gen[(op)->reg1])                                     \
     : (((op)->adr == V0_DIR_ADR)                                       \
        ? ((op)->arg[0].i32 & (1 << ((op)->parm * CHAR_BIT)))           \
        : V0_ADR_INVAL))

/* count for shl, ahr, sar */
#define v0getcnt(vm, op)                                                \
    (((op)->adr == V0_DIR_ADR)                                          \
     ? ((op)->val)                                                      \
     : (((op)->adr == V0_REG_ADR)                                       \
        ? ((vm)->regs.gen[(op)->reg1])                                  \
        : V0_CNT_INVAL))

/* argument count for cpl */
#define v0getparmcnt(vm, op)                                            \
    ((op)->val)

/* argument table address for cpl; NOTE: NO indexed/indirect addresses */
#define v0getparmadr(vm, op)                                            \
    (((op)-> adr == V0_REG_ADR)                                         \
     ? ((vm)->regs.gen[(op)->reg2])                                     \
     : (((op)->adr == V0_DIR_ADR)                                       \
        ? ((vm)->mem[(op)->arg[0].adr])                                 \
        : V0_ADR_INVAL))

/* operand address for psh */
#define v0getsrcadr(vm, op)                                             \
    (((op)->adr == V0_REG_ADR)                                          \
     ? ((vm)->regs.gen[op->reg1])                                       \
     : (((op)->adr == V0_DIR_ADR)                                       \
        ? ((vm)->mem[(op)->arg[0].adr])                                 \
        : V0_ADR_INVAL))

/*
 * V0_REG_ADR -> *reg
 * V0_NDX_ADR -> reg[op->val << op->parm]
 * V0_DIR_ADR -> op->arg.adr
 */
#define v0getadr(vm, op, reg)                                           \
    (((op)->adr == V0_REG_ADR)                                          \
     ? ((vm)->regs.gen[(op)->reg])                                      \
     : (((op)->adr == V0_NDX_ADR)                                       \
        ? ((vm)->regs.gen[(op)->reg] + ((v0reg)(op)->val << (op)->parm))       \
        : (((op)->adr == V0_DIR_ADR)                                    \
           ? ((op)->arg[0].adr)                                         \
           : V0_ADR_INVAL)))

/*
 * V0_REG_ADR -> *reg
 * V0_NDX_ADR -> reg[op->val << op->parm]
 * V0_DIR_ADR -> *(op->arg.adr)
 * V0_PIC_ADR -> pc[op->val << op->parm]
 */
#define v0getjmpadr(vm, op, reg)                                        \
    (((op)->adr == V0_REG_ADR)                                          \
     ? ((vm)->regs.gen[(op)->reg])                                      \
     : (((op)->adr == V0_NDX_ADR)                                       \
        ? ((vm)->regs.gen[(op)->reg] + ((v0reg)(op)->val << (op)->parm)) \
        : (((op)->adr == V0_DIR_ADR)                                    \
           ? ((op)->arg[0].adr)                                         \
           : ((vm)->regs.gen[V0_PC_REG] + ((v0reg)(op)->val << (op)->parm)))))

#define v0getioport(op)                                                 \
    ((op)->val)

static _V0OP_T
v0nop(struct v0 *vm, void *ptr)
{
    v0ureg  pc = vm->regs.sys[V0_PC_REG];
    void   *opadr;

    pc += sizeof(struct v0op);
    v0addspeedcnt(1);
    opadr = v0adrtoptr(vm, pc);
    vm->regs.sys[V0_PC_REG] = pc;

    return opadr;
}

static _V0OP_T
v0not(struct v0 *vm, void *ptr)
{
    struct v0regs *regs = &vm->regs;
    v0ureg         pc = regs->sys[V0_PC_REG];
    void          *opadr;
    struct v0op   *op = ptr;
    v0reg         *dptr = &regs->gen[op->reg1];
    v0reg          src = *dptr;
    v0reg          dest;

    dest = ~src;
    pc += sizeof(struct v0op);
    v0addspeedcnt(1);
    *dptr = dest;
    opadr = v0adrtoptr(vm, pc);
    regs->sys[V0_PC_REG] = pc;

    return opadr;
}

static _V0OP_T
v0and(struct v0 *vm, void *ptr)
{
    struct v0regs *regs = &vm->regs;
    v0ureg         pc = regs->sys[V0_PC_REG];
    void          *opadr;
    struct v0op   *op = ptr;
    v0reg         *dptr = &regs->gen[op->reg2];
    v0reg          src = v0getsrc(vm, op);
    v0reg          dest = *dptr;

    dest &= src;
    pc += sizeof(struct v0op);
    v0addspeedcnt(2);
    *dptr = dest;
    opadr = v0adrtoptr(vm, pc);
    regs->sys[V0_PC_REG] = pc;

    return opadr;
}

static _V0OP_T
v0or(struct v0 *vm, void *ptr)
{
    struct v0regs *regs = &vm->regs;
    v0ureg         pc = regs->sys[V0_PC_REG];
    void          *opadr;
    struct v0op   *op = ptr;
    v0reg         *dptr = &regs->gen[op->reg2];
    v0reg          src = v0getsrc(vm, op);
    v0reg          dest = *dptr;

    dest |= src;
    pc += sizeof(struct v0op);
    v0addspeedcnt(2);
    *dptr = dest;
    opadr = v0adrtoptr(vm, pc);
    regs->sys[V0_PC_REG] = pc;

    return opadr;
}

static _V0OP_T
v0xor(struct v0 *vm, void *ptr)
{
    struct v0regs *regs = &vm->regs;
    v0ureg         pc = regs->sys[V0_PC_REG];
    void          *opadr;
    struct v0op   *op = ptr;
    v0reg         *dptr = &regs->gen[op->reg2];
    v0reg          src = v0getsrc(vm, op);
    v0reg          dest = *dptr;

    dest ^= src;
    pc += sizeof(struct v0op);
    v0addspeedcnt(2);
    *dptr = dest;
    opadr = v0adrtoptr(vm, pc);
    regs->sys[V0_PC_REG] = pc;

    return opadr;
}

static _V0OP_T
v0shl(struct v0 *vm, void *ptr)
{
    struct v0regs *regs = &vm->regs;
    v0ureg         pc = regs->sys[V0_PC_REG];
    void          *opadr;
    struct v0op   *op = ptr;
    v0reg         *dptr = &regs->gen[op->reg2];
    v0reg          src = v0getcnt(vm, op);
    v0reg          dest = *dptr;

    dest <<= src;
    pc += sizeof(struct v0op);
    v0addspeedcnt(4);
    *dptr = dest;
    opadr = v0adrtoptr(vm, pc);
    regs->sys[V0_PC_REG] = pc;

    return opadr;
}

static _V0OP_T
v0shr(struct v0 *vm, void *ptr)
{
    struct v0regs *regs = &vm->regs;
    v0ureg         pc = regs->sys[V0_PC_REG];
    void          *opadr;
    struct v0op   *op = ptr;
    v0reg         *dptr = &regs->gen[op->reg2];
    v0reg          dest = *dptr;
    v0reg          src = v0getcnt(vm, op);
    v0reg          fill = ~((v0reg)0) >> src;

    dest >>= src;
    pc += sizeof(struct v0op);
    v0addspeedcnt(4);
    dest &= fill;
    *dptr = dest;
    opadr = v0adrtoptr(vm, pc);
    regs->sys[V0_PC_REG] = pc;

    return opadr;
}

static _V0OP_T
v0sar(struct v0 *vm, void *ptr)
{
    struct v0regs *regs = &vm->regs;
    v0ureg         pc = regs->sys[V0_PC_REG];
    void          *opadr;
    struct v0op   *op = ptr;
    v0reg         *dptr = &regs->gen[op->reg2];
    v0reg          src = v0getcnt(vm, op);
    v0reg          dest = *dptr;
    v0reg          mask = ~(v0reg)0;
#if (WORDSIZE == 8)
    v0reg          fill = (((dest) & (INT64_C(1) << 63))
                          ? (mask >> (64 - src))
                          : 0);
#else
    v0reg          fill = (((dest) & (INT32_C(1) << 31))
                          ? (mask >> (32 - src))
                          : 0);
#endif

    dest >>= src;
    fill = -fill << (CHAR_BIT * sizeof(v0reg) - src);
    pc += sizeof(struct v0op);
    v0addspeedcnt(4);
    dest &= fill;
    *dptr = dest;
    opadr = v0adrtoptr(vm, pc);
    regs->sys[V0_PC_REG] = pc;

    return opadr;
}

static _V0OP_T
v0inc(struct v0 *vm, void *ptr)
{
    struct v0regs *regs = &vm->regs;
    v0ureg         pc = regs->sys[V0_PC_REG];
    void          *opadr;
    struct v0op   *op = ptr;
    v0reg         *dptr = &regs->gen[op->reg1];
    v0reg          src = *dptr;

    src++;
    pc += sizeof(struct v0op);
    v0addspeedcnt(2);
    *dptr = src;
    opadr = v0adrtoptr(vm, pc);
    regs->sys[V0_PC_REG] = pc;

    return opadr;
}

static _V0OP_T
v0dec(struct v0 *vm, void *ptr)
{
    struct v0regs *regs = &vm->regs;
    v0ureg         pc = regs->sys[V0_PC_REG];
    void          *opadr;
    struct v0op   *op = ptr;
    v0reg        *dptr = &regs->gen[op->reg1];
    v0reg         src = *dptr;

    src--;
    pc += sizeof(struct v0op);
    v0addspeedcnt(2);
    *dptr = src;
    opadr = v0adrtoptr(vm, pc);
    regs->sys[V0_PC_REG] = pc;

    return opadr;
}

static _V0OP_T
v0add(struct v0 *vm, void *ptr)
{
    struct v0regs *regs = &vm->regs;
    v0ureg         pc = regs->sys[V0_PC_REG];
    void          *opadr;
    struct v0op   *op = ptr;
    v0reg        *dptr = &regs->gen[op->reg2];
    v0reg         src = v0getsrc(vm, op);
    v0reg         dest = *dptr;
    v0reg         res;

    pc += sizeof(struct v0op);
    res = src + dest;
    v0addspeedcnt(4);
    if (res < dest) {
        v0setof(vm);
    }
    *dptr = dest;
    opadr = v0adrtoptr(vm, pc);
    regs->sys[V0_PC_REG] = pc;

    return opadr;
}

/* FIXME: set carry-bit */
static _V0OP_T
v0adc(struct v0 *vm, void *ptr)
{
    struct v0regs *regs = &vm->regs;
    v0ureg         pc = regs->sys[V0_PC_REG];
    void          *opadr;
    struct v0op   *op = ptr;
    v0reg        *dptr = &regs->gen[op->reg2];
    v0reg         src = v0getsrc(vm, op);
    v0reg         dest = *dptr;
    v0reg         res;

    pc += sizeof(struct v0op);
    res = src + dest;
    v0addspeedcnt(4);
    if (res < dest) {
        v0setof(vm);
    }
    *dptr = dest;
    opadr = v0adrtoptr(vm, pc);
    regs->sys[V0_PC_REG] = pc;

    return opadr;
}

static _V0OP_T
v0sub(struct v0 *vm, void *ptr)
{
    struct v0regs *regs = &vm->regs;
    v0ureg         pc = regs->sys[V0_PC_REG];
    void          *opadr;
    struct v0op   *op = ptr;
    v0reg         *dptr = &regs->gen[op->reg2];
    v0reg          src = v0getsrc(vm, op);
    v0reg          dest = *dptr;

    pc += sizeof(struct v0op);
    dest -= src;
    v0addspeedcnt(4);
    *dptr = dest;
    opadr = v0adrtoptr(vm, pc);
    regs->sys[V0_PC_REG] = pc;

    return opadr;
}

static _V0OP_T
v0sbb(struct v0 *vm, void *ptr)
{
    struct v0regs *regs = &vm->regs;
    v0ureg         pc = regs->sys[V0_PC_REG];
    void          *opadr;
    struct v0op   *op = ptr;
    v0reg         *dptr = &regs->gen[op->reg2];
    v0reg          src = v0getsrc(vm, op);
    v0reg          dest = *dptr;

    pc += sizeof(struct v0op);
    dest -= src;
    v0addspeedcnt(4);
    opadr = v0adrtoptr(vm, pc);
    *dptr = dest;
    regs->sys[V0_PC_REG] = pc;

    return opadr;
}

static _V0OP_T
v0cmp(struct v0 *vm, void *ptr)
{
    struct v0regs *regs = &vm->regs;
    v0ureg         pc = regs->sys[V0_PC_REG];
    void          *opadr;
    struct v0op   *op = ptr;
    v0reg         *dptr = &regs->gen[op->reg2];
    v0reg          src = v0getsrc(vm, op);
    v0reg          dest = *dptr;

    pc += sizeof(struct v0op);
    dest -= src;
    v0addspeedcnt(8);
    v0clrmsw(vm);
    if (!dest) {
        v0setzf(vm);
    } else if (dest < 0) {
        v0setcf(vm);
    }
    opadr = v0adrtoptr(vm, pc);
    *dptr = dest;
    regs->sys[V0_PC_REG] = pc;

    return opadr;
}

static _V0OP_T
v0mul(struct v0 *vm, void *ptr)
{
    struct v0regs *regs = &vm->regs;
    v0ureg         pc = regs->sys[V0_PC_REG];
    void          *opadr;
    struct v0op   *op = ptr;
    v0reg         *dptr = &regs->gen[op->reg2];
    v0reg          src = v0getsrc(vm, op);
    v0reg          dest = *dptr;

    pc += sizeof(struct v0op);
    dest *= src;
    v0addspeedcnt(16);
    *dptr = dest;
    opadr = v0adrtoptr(vm, pc);
    regs->sys[V0_PC_REG] = pc;

    return opadr;
}

static _V0OP_T
v0div(struct v0 *vm, void *ptr)
{
    struct v0regs *regs = &vm->regs;
    v0ureg         pc = regs->sys[V0_PC_REG];
    void          *opadr;
    struct v0op   *op = ptr;
    v0reg         *dptr = &regs->gen[op->reg2];
    v0reg          src = v0getsrc(vm, op);
    v0reg          dest = *dptr;

    pc += sizeof(struct v0op);
    if (!src) {
        v0doxcpt(V0_DIV_BY_ZERO);
    } else if (src <= 0xffff) {
        dest = fastuf16divuf16(dest, src, vm->divu16tab);
    } else {
        dest /= src;
    }
    v0addspeedcnt(64);
    *dptr = dest;
    opadr = v0adrtoptr(vm, pc);
    regs->sys[V0_PC_REG] = pc;

    return opadr;
}

static _V0OP_T
v0rem(struct v0 *vm, void *ptr)
{
    struct v0regs *regs = &vm->regs;
    v0ureg         pc = regs->sys[V0_PC_REG];
    void          *opadr;
    struct v0op   *op = ptr;
    v0reg         *dptr = &regs->gen[op->reg2];
    v0reg          src = v0getsrc(vm, op);
    v0reg          dest = *dptr;

    pc += sizeof(struct v0op);
    dest %= src;
    v0addspeedcnt(64);
    *dptr = dest;
    opadr = v0adrtoptr(vm, pc);
    regs->sys[V0_PC_REG] = pc;

    return opadr;
}

static _V0OP_T
v0jmp(struct v0 *vm, void *ptr)
{
    struct v0regs *regs = &vm->regs;
    v0ureg         pc = regs->sys[V0_PC_REG];
    void          *opadr;
    struct v0op   *op = ptr;
    v0reg          dest = v0getjmpadr(vm, op, reg1);

    pc = dest;
    v0addspeedcnt(8);
    opadr = v0adrtoptr(vm, pc);
    regs->sys[V0_PC_REG] = pc;

    return opadr;
}

/* cpl(cnt, tab) - Call ProLogue
 * ---
 * - push caller-save registers
 * - load up to 6 arguments into registers r0..r5
 * - store rest of arguments to stack
 * - load argument count into register r6 aka ac
 * - load local variable count into r7 aka vc
 */
/* stack after cpl
 * ---------------
 * ...
 * r7
 * r6
 * r5
 * r4
 * r3
 * r2
 * r1
 * r0
 * argN
 * ...
 * arg0 <- sp
 */

static _V0OP_T
v0cpl(struct v0 *vm, void *ptr)
{
    struct v0regs *regs = &vm->regs;
    v0ureg         pc = regs->sys[V0_PC_REG];
    void          *opadr;
    struct v0op   *op = ptr;
    v0ureg         cnt = v0getparmcnt(vm, op);
    v0ureg         tab = v0getparmadr(vm, op);
    v0ureg         sp = regs->sys[V0_SP_REG];
    v0reg         *fptr = v0adrtoptr(vm, tab);
    v0reg         *sptr = v0adrtoptr(vm, sp);
    v0reg         *rptr = &regs->gen[V0_R0_REG];
    v0ureg         ac = fptr[0];
    v0ureg         vc = fptr[1];
    v0ureg         ndx;

    v0addspeedcnt(16);
    sptr -= V0_NSAVEREG;
    sp -= V0_NSAVEREG * sizeof(v0reg);
    fptr += 8;
    /* save caller-save registers r0..r7 on stack */
    sptr[V0_R0_REG] = rptr[V0_R0_REG];
    sptr[V0_R1_REG] = rptr[V0_R1_REG];
    sptr[V0_R2_REG] = rptr[V0_R2_REG];
    sptr[V0_R3_REG] = rptr[V0_R3_REG];
    sptr[V0_R4_REG] = rptr[V0_R4_REG];
    sptr[V0_R5_REG] = rptr[V0_R5_REG];
    sptr[V0_R6_REG] = rptr[V0_R6_REG];
    sptr[V0_R7_REG] = rptr[V0_R7_REG];
    if (ac) {
        /* store rest of arguments to stack in reverse order */
        cnt = ac;
        sptr -= ac;
        ndx = 0;
        sp -= ac * sizeof(v0reg);
        v0addspeedcnt(32);
        while (cnt--) {
            sptr[ndx] = fptr[ndx];
            ndx++;
        }
    }
    fptr -= 6;
    opadr = v0adrtoptr(vm, pc);
    pc += sizeof(struct v0op);
    /* load registers r0..r5 from argument table */
    switch (cnt) {
        default:
            rptr[V0_R5_REG] = fptr[5];
        case 5:
            rptr[V0_R4_REG] = fptr[4];
        case 4:
            rptr[V0_R3_REG] = fptr[3];
        case 3:
            rptr[V0_R2_REG] = fptr[2];
        case 2:
            rptr[V0_R1_REG] = fptr[1];
        case 1:
            rptr[V0_R0_REG] = fptr[0];
        case 0:

            break;
    }
    regs->sys[sp] = sp;
    regs->gen[V0_AC_REG] = ac;
    regs->gen[V0_VC_REG] = vc;
    regs->sys[V0_PC_REG] = pc;

    return opadr;
}

/* call
 * ----
 *
 * prologue; TODO: mkf (construct stack frame)
 * --------
 * call
 * ----
 * - push LN (link register / return address)
 *
 * stack frame after call
 * ----------------------
 * r7
 * r6
 * r5
 * r4
 * r3
 * r2
 * r1
 * r0
 * argN
 * ...
 * arg0
 * ln <- sp
 */
static _V0OP_T
v0call(struct v0 *vm, void *ptr)
{
    struct v0regs *regs = &vm->regs;
    v0ureg         pc = regs->sys[V0_PC_REG];
    void          *opadr;
    struct v0op   *op = ptr;
    v0ureg         sp = regs->sys[V0_SP_REG];
    v0ureg         ln = regs->sys[V0_LN_REG];
    v0ureg         dest = v0getjmpadr(vm, op, reg1);
    v0reg         *sptr;

    sp -= sizeof(v0reg);
    pc += sizeof(struct v0op);
    sptr = v0adrtoptr(vm, sp);
    if (op->adr == V0_REG_ADR) {
        v0addspeedcnt(4);
    } else {
        v0addspeedcnt(8);
    }
    *sptr = ln;
    regs->sys[V0_SP_REG] = sp;
    opadr = v0adrtoptr(vm, dest);
    regs->sys[V0_LN_REG] = pc;
    regs->sys[V0_PC_REG] = dest;

    return opadr;
}

/* create subroutine stack-frame;
 * - push frame pointer
 * - push callee-save registers
 * - allocate room for local variables on stack
 */
/*
 * stack after enter
 * -----------------
 * ln
 * oldfp <- fp
 * var0
 * ...
 * varN <- sp
 */
static _V0OP_T
v0enter(struct v0 *vm, void *ptr)
{
    struct v0regs *regs = &vm->regs;
    v0ureg         pc = regs->sys[V0_PC_REG];
    void          *opadr;
    v0ureg         fp = regs->sys[V0_SP_REG];
    v0ureg         sp = regs->sys[V0_SP_REG];
    v0ureg         vc = regs->sys[V0_VC_REG];
    v0reg         *sptr;
    v0reg         *rptr = &regs->gen[V0_R0_REG];

    /* set stack frame up */
    sp -= sizeof(v0reg);
    v0addspeedcnt(32);
    sptr = v0adrtoptr(vm, sp);
    regs->sys[V0_FP_REG] = sp;
    *sptr = fp;
    pc += sizeof(struct v0op);
    sptr -= V0_NSAVEREG;
    sp -= V0_NSAVEREG * sizeof(v0reg);
    rptr[V0_R8_REG] = sptr[V0_R8_REG];
    rptr[V0_R9_REG] = sptr[V0_R9_REG];
    rptr[V0_R10_REG] = sptr[V0_R10_REG];
    rptr[V0_R11_REG] = sptr[V0_R11_REG];
    rptr[V0_R12_REG] = sptr[V0_R12_REG];
    rptr[V0_R13_REG] = sptr[V0_R13_REG];
    rptr[V0_R14_REG] = sptr[V0_R14_REG];
    rptr[V0_R15_REG] = sptr[V0_R15_REG];
    sp -= vc * sizeof(v0reg);
    opadr = v0adrtoptr(vm, pc);
    regs->sys[V0_SP_REG] = sp;
    regs->sys[V0_PC_REG] = pc;

    return opadr;
}

/* destroy subroutine stack-frame */
/*
 * stack after leave
 * -----------------
 * - deallocate local variables
 * - pop caller frame pointer
 * ln <- sp
 * oldfp
 * local variables
 */
static _V0OP_T
v0leave(struct v0 *vm, void *ptr)
{
    struct v0regs *regs = &vm->regs;
    v0ureg         pc = regs->sys[V0_PC_REG];
    void          *opadr;
    v0ureg         fp = regs->sys[V0_FP_REG];
    v0ureg         sp;
    v0reg         *fptr = v0adrtoptr(vm, fp);

    sp = fp;
    pc += sizeof(struct v0op);
    v0addspeedcnt(8);
    fp = *fptr;
    opadr = v0adrtoptr(vm, pc);
    regs->sys[V0_SP_REG] = sp;
    regs->sys[V0_FP_REG] = fp;
    regs->sys[V0_PC_REG] = pc;

    return opadr;
}

/* return from subroutine;
 * - get old link-register value from stack (LN)
 * - adjust stack pointer (SP) to frame pointer (FP)
 */
static _V0OP_T
v0ret(struct v0 *vm, void *ptr)
{
    struct v0regs *regs = &vm->regs;
    void          *opadr;
    v0ureg         sp = regs->sys[V0_SP_REG];
    v0reg         *sptr = v0adrtoptr(vm, sp);
    v0ureg         ln;

    v0addspeedcnt(16);
    sp -= sizeof(v0reg);
    ln = *sptr;
    regs->sys[V0_SP_REG] = sp;
    opadr = v0adrtoptr(vm, ln);
    regs->sys[V0_PC_REG] = ln;

    return opadr;
}

static _V0OP_T
v0bz(struct v0 *vm, void *ptr)
{
    struct v0regs *regs = &vm->regs;
    v0ureg         pc = regs->sys[V0_PC_REG];
    void          *opadr;
    struct v0op   *op = ptr;
    v0reg          dest = v0getjmpadr(vm, op, reg1);

    if (v0zfset(vm)) {
        pc = dest;
    } else {
        pc += sizeof(struct v0op);
    }
    v0addspeedcnt(16);
    opadr = v0adrtoptr(vm, pc);
    regs->sys[V0_PC_REG] = pc;

    return opadr;
}

static _V0OP_T
v0bnz(struct v0 *vm, void *ptr)
{
    struct v0regs *regs = &vm->regs;
    v0ureg         pc = regs->sys[V0_PC_REG];
    void          *opadr;
    struct v0op   *op = ptr;
    v0reg          dest = v0getjmpadr(vm, op, reg1);

    if (!v0zfset(vm)) {
        pc = dest;
    } else {
        pc += sizeof(struct v0op);
    }
    v0addspeedcnt(16);
    opadr = v0adrtoptr(vm, pc);
    regs->sys[V0_PC_REG] = pc;

    return opadr;
}

static _V0OP_T
v0bc(struct v0 *vm, void *ptr)
{
    struct v0regs *regs = &vm->regs;
    v0ureg         pc = regs->sys[V0_PC_REG];
    void          *opadr;
    struct v0op   *op = ptr;
    v0reg          dest = v0getjmpadr(vm, op, reg1);

    if (v0cfset(vm)) {
        pc = dest;
    } else {
        pc += sizeof(struct v0op);
    }
    v0addspeedcnt(16);
    opadr = v0adrtoptr(vm, pc);
    regs->sys[V0_PC_REG] = pc;

    return opadr;
}

static _V0OP_T
v0bnc(struct v0 *vm, void *ptr)
{
    struct v0regs *regs = &vm->regs;
    v0ureg         pc = regs->sys[V0_PC_REG];
    void          *opadr;
    struct v0op   *op = ptr;
    v0reg          dest = v0getjmpadr(vm, op, reg1);

    if (!v0cfset(vm)) {
        pc = dest;
    } else {
        pc += sizeof(struct v0op);
    }
    v0addspeedcnt(16);
    opadr = v0adrtoptr(vm, pc);
    regs->sys[V0_PC_REG] = pc;

    return opadr;
}

static _V0OP_T
v0bo(struct v0 *vm, void *ptr)
{
    struct v0regs *regs = &vm->regs;
    v0ureg         pc = regs->sys[V0_PC_REG];
    void          *opadr;
    struct v0op   *op = ptr;
    v0reg          dest = v0getjmpadr(vm, op, reg1);

    if (v0ofset(vm)) {
        pc = dest;
    } else {
        pc += sizeof(struct v0op);
    }
    v0addspeedcnt(16);
    opadr = v0adrtoptr(vm, pc);
    regs->sys[V0_PC_REG] = pc;

    return opadr;
}

static _V0OP_T
v0bno(struct v0 *vm, void *ptr)
{
    struct v0regs *regs = &vm->regs;
    v0ureg         pc = regs->sys[V0_PC_REG];
    void          *opadr;
    struct v0op   *op = ptr;
    v0reg          dest = v0getjmpadr(vm, op, reg1);

    if (!v0ofset(vm)) {
        pc = dest;
    } else {
        pc += sizeof(struct v0op);
    }
    v0addspeedcnt(16);
    opadr = v0adrtoptr(vm, pc);
    regs->sys[V0_PC_REG] = pc;

    return opadr;
}

static _V0OP_T
v0blt(struct v0 *vm, void *ptr)
{
    struct v0regs *regs = &vm->regs;
    v0ureg         pc = regs->sys[V0_PC_REG];
    void          *opadr;
    struct v0op   *op = ptr;
    v0reg          dest = v0getjmpadr(vm, op, reg1);

    if (!v0ofset(vm)) {
        pc = dest;
    } else {
        pc += sizeof(struct v0op);
    }
    v0addspeedcnt(16);
    opadr = v0adrtoptr(vm, pc);
    regs->sys[V0_PC_REG] = pc;

    return opadr;
}

static _V0OP_T
v0ble(struct v0 *vm, void *ptr)
{
    struct v0regs *regs = &vm->regs;
    v0ureg         pc = regs->sys[V0_PC_REG];
    void          *opadr;
    struct v0op   *op = ptr;
    v0reg          dest = v0getjmpadr(vm, op, reg1);

    pc += sizeof(struct v0op);
    if (!v0ofset(vm) || v0zfset(vm)) {
        ptr = v0adrtoptr(vm, dest);
    } else {
        ptr = v0adrtoptr(vm, pc);
    }
    v0addspeedcnt(16);
    opadr = v0adrtoptr(vm, pc);
    regs->sys[V0_PC_REG] = pc;

    return opadr;
}

static _V0OP_T
v0bgt(struct v0 *vm, void *ptr)
{
    struct v0regs *regs = &vm->regs;
    v0ureg         pc = regs->sys[V0_PC_REG];
    void          *opadr;
    struct v0op   *op = ptr;
    v0reg          dest = v0getjmpadr(vm, op, reg1);

    if (v0ofset(vm)) {
        pc = dest;
    } else {
        pc += sizeof(struct v0op);
    }
    v0addspeedcnt(16);
    opadr = v0adrtoptr(vm, pc);
    regs->sys[V0_PC_REG] = pc;

    return opadr;
}

static _V0OP_T
v0bge(struct v0 *vm, void *ptr)
{
    struct v0regs *regs = &vm->regs;
    v0ureg         pc = regs->sys[V0_PC_REG];
    void          *opadr;
    struct v0op   *op = ptr;
    v0reg          dest = v0getjmpadr(vm, op, reg1);

    pc += sizeof(struct v0op);
    if (v0ofset(vm) || v0zfset(vm)) {
        ptr = v0adrtoptr(vm, dest);
    } else {
        ptr = v0adrtoptr(vm, pc);
    }
    v0addspeedcnt(16);
    opadr = v0adrtoptr(vm, pc);
    regs->sys[V0_PC_REG] = pc;

    return opadr;
}

static _V0OP_T
v0ldr(struct v0 *vm, void *ptr)
{
    struct v0regs *regs = &vm->regs;
    v0ureg         pc = regs->sys[V0_PC_REG];
    void          *opadr;
    struct v0op   *op = ptr;
    v0reg          adr = v0getadr(vm, op, reg1);
    v0reg          src = 0;
    v0ureg         usrc;
    v0reg          parm = op->parm;
    v0reg         *dptr = &regs->gen[op->reg2];
    v0reg         *sptr;
    v0reg          mask;

    if (!adr) {
        v0doxcpt(V0_INV_MEM_ADR);
    }
    pc += sizeof(struct v0op);
    if (op->adr == V0_REG_ADR) {
        mask = 1 << (parm * CHAR_BIT);
        sptr = &regs->gen[op->reg1];
        mask--;
        src = *sptr;
        v0addspeedcnt(4);
        src &= mask;
        *dptr |= src;
    } else if (op->adr == V0_DIR_ADR) {
        v0addspeedcnt(16);
        pc += sizeof(union v0oparg);
        if (v0opissigned(op)) {
            switch (parm) {
                case 0:
                    src = *(int8_t *)v0adrtoptr(vm, adr);

                    break;
                case 1:
                    src = *(int16_t *)v0adrtoptr(vm, adr);

                    break;
                case 2:
                    src = *(int32_t *)v0adrtoptr(vm, adr);

                    break;
                case 3:
                    v0doxcpt(V0_INV_MEM_READ);

                    break;
            }
            *dptr = src;
        } else {
            switch (op->parm) {
                case 0:
                    usrc = *(uint8_t *)v0adrtoptr(vm, adr);

                    break;
                case 1:
                    usrc = *(uint16_t *)v0adrtoptr(vm, adr);

                    break;
                case 2:
                    usrc = *(uint32_t *)v0adrtoptr(vm, adr);

                    break;
                case 3:
                    v0doxcpt(V0_INV_MEM_READ);

                    break;
            }
            *(v0ureg *)dptr = usrc;
        }
    }
    opadr = v0adrtoptr(vm, pc);
    regs->sys[V0_PC_REG] = pc;

    return opadr;
}

static _V0OP_T
v0str(struct v0 *vm, void *ptr)
{
    struct v0regs *regs = &vm->regs;
    v0ureg         pc = regs->sys[V0_PC_REG];
    void          *opadr;
    struct v0op   *op = ptr;
    v0reg          adr = v0getadr(vm, op, reg2);
    v0reg          src;
    v0ureg         usrc;
    v0reg          parm = op->parm;
    v0reg         *dptr;
    v0reg         *sptr = v0adrtoptr(vm, op->reg1);
    v0reg          mask;

    if (!adr) {
        v0doxcpt(V0_INV_MEM_ADR);
    }
    pc += sizeof(struct v0op);
    if (op->adr == V0_REG_ADR) {
        mask = 1 << (parm * CHAR_BIT);
        v0addspeedcnt(4);
        mask--;
        src = *sptr;
        dptr = &regs->gen[op->reg2];
        src &= mask;
        *dptr |= src;
    } else if (op->adr == V0_DIR_ADR) {
        v0addspeedcnt(16);
        pc += sizeof(union v0oparg);
        if (v0opissigned(op)) {
            switch (op->parm) {
                case 0:
                    src = *(int8_t *)v0adrtoptr(vm, adr);
                    *(int8_t *)v0adrtoptr(vm, adr) = (int8_t)src;

                    break;
                case 1:
                    src = *(int16_t *)v0adrtoptr(vm, adr);
                    *(int16_t *)v0adrtoptr(vm, adr) = (int16_t)src;

                    break;
                case 2:
                    src = *(int32_t *)v0adrtoptr(vm, adr);
                    *(int32_t *)v0adrtoptr(vm, adr) = (int32_t)src;

                    break;
                case 3:
                    v0doxcpt(V0_INV_MEM_WRITE);

                    break;
            }
        } else {
            switch (op->parm) {
                case 0:
                    usrc = *(uint8_t *)v0adrtoptr(vm, adr);
                    *(uint8_t *)v0adrtoptr(vm, adr) = (uint8_t)usrc;

                    break;
                case 1:
                    usrc = *(uint16_t *)v0adrtoptr(vm, adr);
                    *(uint16_t *)v0adrtoptr(vm, adr) = (uint16_t)usrc;

                    break;
                case 2:
                    usrc = *(int32_t *)v0adrtoptr(vm, adr);
                    *(uint32_t *)v0adrtoptr(vm, adr) = (uint32_t)usrc;

                    break;
                case 3:
                    v0doxcpt(V0_INV_MEM_WRITE);

                    break;
            }
        }
    }
    opadr = v0adrtoptr(vm, pc);
    regs->sys[V0_PC_REG] = pc;

    return opadr;
}

static _V0OP_T
v0psh(struct v0 *vm, void *ptr)
{
    struct v0regs *regs = &vm->regs;
    v0ureg         pc = regs->sys[V0_PC_REG];
    void          *opadr;
    struct v0op   *op = ptr;
    v0reg          src = v0getsrcadr(vm, op);
    v0ureg         sp = regs->sys[V0_SP_REG];
    v0reg         *sptr = v0adrtoptr(vm, src);
    v0reg         *dptr = v0adrtoptr(vm, sp);

    sp -= sizeof(v0reg);
    pc += sizeof(struct v0op);
    if (op->adr == V0_REG_ADR) {
        v0addspeedcnt(4);
    } else if (op->adr == V0_DIR_ADR) {
        v0addspeedcnt(8);
        pc += sizeof(union v0oparg);
    }
    *dptr = *sptr;
    regs->sys[V0_SP_REG] = sp;
    opadr = v0adrtoptr(vm, pc);
    regs->sys[V0_PC_REG] = pc;

    return opadr;
}

static _V0OP_T
v0pop(struct v0 *vm, void *ptr)
{
    struct v0regs *regs = &vm->regs;
    v0ureg         pc = regs->sys[V0_PC_REG];
    void          *opadr;
    struct v0op   *op = ptr;
    v0ureg         sp = regs->sys[V0_SP_REG];
    v0reg         *src = v0adrtoptr(vm, sp);
    v0reg         *dest = &regs->gen[op->reg1];

    pc += sizeof(struct v0op);
    sp += sizeof(v0reg);
    *dest = *src;
    v0addspeedcnt(4);
    regs->sys[V0_SP_REG] = sp;
    opadr = v0adrtoptr(vm, pc);
    regs->sys[V0_PC_REG] = pc;

    return opadr;
}

static _V0OP_T
v0psha(struct v0 *vm, void *ptr)
{
    struct v0regs *regs = &vm->regs;
    v0ureg         pc = regs->sys[V0_PC_REG];
    void          *opadr;
    v0ureg         sp = regs->sys[V0_SP_REG];
    v0reg         *src = &regs->gen[V0_R0_REG];
    v0reg         *dest = v0adrtoptr(vm, sp);
    v0reg          ndx;

    pc += sizeof(struct v0op);
    sp -= V0_NGENREG * sizeof(v0reg);
    v0addspeedcnt(16);
    dest -= V0_NGENREG;
    for (ndx = 0 ; ndx < (V0_NGENREG >> 3); ndx++) {
        dest[0] = src[0];
        dest[1] = src[1];
        dest[2] = src[2];
        dest[3] = src[3];
        dest[4] = src[4];
        dest[5] = src[5];
        dest[6] = src[6];
        dest[7] = src[7];
        sp += 8;
        dest += 8;
        src += 8;
    }
    regs->sys[V0_SP_REG] = sp;
    opadr = v0adrtoptr(vm, pc);
    regs->sys[V0_PC_REG] = pc;

    return opadr;
}

static _V0OP_T
v0popa(struct v0 *vm, void *ptr)
{
    struct v0regs *regs = &vm->regs;
    v0ureg         pc = regs->sys[V0_PC_REG];
    void          *opadr;
    v0ureg         sp = regs->sys[V0_SP_REG];
    v0reg         *dest = &regs->gen[V0_R0_REG];
    v0reg         *src = v0adrtoptr(vm, sp);
    v0reg          ndx;

    v0addspeedcnt(32);
    pc += sizeof(struct v0op);
    sp -= V0_NGENREG * sizeof(v0reg);
    for (ndx = 0 ; ndx < (V0_NGENREG >> 3); ndx++) {
        dest[0] = src[0];
        dest[1] = src[1];
        dest[2] = src[2];
        dest[3] = src[3];
        dest[4] = src[4];
        dest[5] = src[5];
        dest[6] = src[6];
        dest[7] = src[7];
        sp += 8;
        dest += 8;
        src += 8;
    }
    regs->sys[V0_SP_REG] = sp;
    opadr = v0adrtoptr(vm, pc);
    regs->sys[V0_PC_REG] = pc;

    return opadr;
}

static _V0OP_T
v0ior(struct v0 *vm, void *ptr)
{
    v0ureg       pc = vm->regs.sys[V0_PC_REG];
    void        *opadr;
    struct v0op *op = ptr;
    uint16_t     port = v0getioport(op);
    uint16_t     reg = op->reg1;
    v0iofunc_t  *func = vm->iovec[port].rdfunc;

    v0addspeedcnt(8);
    pc += sizeof(struct v0op);
    if (func) {
        func(vm, port, reg);
    } else {
        v0doxcpt(V0_INV_IO_READ);
    }
    opadr = v0adrtoptr(vm, pc);
    vm->regs.sys[V0_PC_REG] = pc;

    return opadr;
}

static _V0OP_T
v0iow(struct v0 *vm, void *ptr)
{
    v0ureg       pc = vm->regs.sys[V0_PC_REG];
    void        *opadr;
    struct v0op *op = ptr;
    uint16_t     port = v0getioport(op);
    v0reg        val = v0getsrc(vm, op);
    v0iofunc_t  *func = vm->iovec[port].wrfunc;

    v0addspeedcnt(8);
    pc += sizeof(struct v0op);
    if (func) {
        func(vm, port, val);
    } else {
        v0doxcpt(V0_INV_IO_WRITE);
    }
    if (op->adr == V0_DIR_ADR) {
        pc += sizeof(union v0oparg);
        v0addspeedcnt(2);
    }
    opadr = v0adrtoptr(vm, pc);
    vm->regs.sys[V0_PC_REG] = pc;

    return opadr;
}

#endif /* __V0_OP_H__ */

