#ifndef __ZPM_ZPM_H__
#define __ZPM_ZPM_H__

#include <zpm/conf.h>
#include <stdint.h>
#include <zero/cdefs.h>

typedef int8_t   zpmbyte;
typedef uint8_t  zpmubyte;
typedef int16_t  zpmword;
typedef uint16_t zpmuword;
typedef int32_t  zpmlong;
typedef uint32_t zpmulong;
#if defined(ZPM64BIT)
typedef int64_t  zpmquad;
typedef uint64_t zpmuquad;
typedef zpmquad  zpmreg;
typedef zpmuquad zpmureg;
#else
typedef zpmlong  zpmreg;
typedef zpmulong zpmureg;
#endif
typedef zpmureg  zpmadr;

/* INSTRUCTION SET */

/* ALU (arithmetic-logical unit) instructions */
/* bitwise operations */
/* logic unit */
#define ZPM_NOT      0x00       // 2's complement (reverse all bits)
#define ZPM_AND      0x01       // logical bitwise AND
#define ZPM_OR       0x02       // logical bitwise OR
#define ZPM_XOR      0x03       // logical bitwise XOR (exclusive OR)
/* shifter */
#define ZPM_SHL      0x04       // shift left
#define ZPM_SHR      0x05       // logical shift right (fill with zero)
#define ZPM_SAR      0x06       // arithmetic shift right (fill with sign-bit)
#define ZPM_ROL      0x07       // rotate left
#define ZPM_ROR      0x08       // rotate right
/* arithmetic operations */
#define ZPM_INC      0x09       // increment by one
#define ZPM_DEC      0x0a       // decrement by one
#define ZPM_ADD      0x0b       // addition
#define ZPM_SUB      0x0c       // subtraction
#define ZPM_CMP      0x0d       // compare; subtract and set flags
#define ZPM_MUL      0x0e       // multiplication
#define ZPM_DIV      0x0f       // division
#define ZPM_REM      0x10       // remainder of division (modulus)
/* branch operations */
#define ZPM_JMP      0x11       // branch unconditionally
#define ZPM_BZ       0x12       // branch if zero (ZF == 1)
#define ZPM_BNZ      0x13       // branch if non-zero (ZF == 0)
#define ZPM_BLT      0x14       // branch if less than
#define ZPM_BLE      0x15       // branch if less than or equal
#define ZPM_BGT      0x16       // branch if greater than
#define ZPM_BGE      0x17       // branch if greater than or equal
#define ZPM_BO       0x18       // branch if overflow set
#define ZPM_BNO      0x19       // branch if overflow not set
#define ZPM_BC       0x1a       // branch if carry set
#define ZPM_BNC      0x1b       // branch if carry not set
/* stack operations */
#define ZPM_POP      0x1c       // pop from stack
#define ZPM_POPA     0x1d       // pop all registers from stack
#define ZPM_PUSH     0x1e       // push on stack
#define ZPM_PUSHA    0x1f       // push all registers on stack
/* load and store operations */
#define ZPM_LDA      0x20       // load accumulator (register)
#define ZPM_STA      0x21       // store accumulator
/* subroutine operations */
#define ZPM_CALL     0x22       // call subroutine
#define ZPM_ENTER    0x23       // subroutine prologue
#define ZPM_LEAVE    0x24       // subroutine epilogue
#define ZPM_RET      0x25       // return from subroutine
/* thread operations */
#define ZPM_THR      0x26       // launch new thread
#define ZPM_LTB      0x27       // load base address + size of thread-local data
/* system operations */
#define ZPM_LDR      0x28       // load special register
#define ZPM_STR      0x29       // store special register
#define ZPM_RST      0x2a       // reset
#define ZPM_HLT      0x2b       // halt
/* I/O operations */
#define ZPM_IN       0x2c       // read data from port
#define ZPM_OUT      0x2d       // write data to port
#define ZPM_NALU_OP  0x2e       // number of ALU operations
/* no operation */
#define ZPM_NOP      0x7f       // no operation
#define ZPM_NALU_RES 128

/* VIRTUAL MACHINE */

#define zpmclrmsw(vm) ((vm)->sysregs[ZPM_MSW] = 0)
#define zpmsetcf(vm)  ((vm)->sysregs[ZPM_MSW] |= ZPM_MSW_CF)
#define zpmsetzf(vm)  ((vm)->sysregs[ZPM_MSW] |= ZPM_MSW_ZF)
#define zpmsetof(vm)  ((vm)->sysregs[ZPM_MSW] |= ZPM_MSW_OF)
#define zpmcfset(vm)  ((vm)->sysregs[ZPM_MSW] & ZPM_MSW_CF)
#define zpmzfset(vm)  ((vm)->sysregs[ZPM_MSW] & ZPM_MSW_ZF)
#define zpmofset(vm)  ((vm)->sysregs[ZPM_MSW] & ZPM_MSW_OF)

/* accumulator (general-purpose register) IDs */
#define ZPM_REG0     0x00
#define ZPM_REG1     0x01
#define ZPM_REG2     0x02
#define ZPM_REG3     0x03
#define ZPM_REG4     0x04
#define ZPM_REG5     0x05
#define ZPM_REG6     0x06
#define ZPM_REG7     0x07
#define ZPM_REG8     0x08
#define ZPM_REG9     0x09
#define ZPM_REG10    0x0a
#define ZPM_REG11    0x0b
#define ZPM_REG12    0x0c
#define ZPM_REG13    0x0d
#define ZPM_REG14    0x0e
#define ZPM_REG15    0x0f
#define ZPM_NGENREG  16
/* system register IDs */
#define ZPM_MSW      0x00       // machine status word
#define ZPM_PC       0x01       // program counter i.e. instruction pointer
#define ZPM_FP       0x02       // frame pointer
#define ZPM_SP       0x03       // stack pointer
#define ZPM_PDB      0x04       // page director base address register
#define ZPM_NSYSREG  16
/* values for sysregs[ZPM_MSW] */
#define ZPM_MSW_CF   (1 << 0)   // carry-flag
#define ZPM_MSW_ZF   (1 << 1)   // zero-flag
#define ZPM_MSW_OF   (1 << 2)   // overflow-flag
#define ZPM_MSW_LF   (1 << 31)  // bus lock flag
/* program segments */
#define ZPM_TEXT     0x00       // code
#define ZPM_RODATA   0x01       // read-only data (string literals etc.)
#define ZPM_DATA     0x02       // read-write (initialised) data
#define ZPM_BSS      0x03       // uninitialised (zeroed) runtime-allocated data
#define ZPM_TLS      0x04       // thread-local storage
#define ZPM_STACK    0x05       // stack
#define ZPM_NSEG     0x08
struct zpm {
    zpmreg   genregs[ZPM_NGENREG];
    zpmureg  sysregs[ZPM_NSYSREG];
    zpmureg *segs[ZPM_NSEG];
    zpmureg  seglims[ZPM_NSEG];
    uint8_t *mem;
};

/* OPCODES */

/* argument type flags */
#define ZPM_MEM_BIT   (1 << 2)  // memory address argument (default is register)
#define ZPM_IMM_ARG    0x01     // immediate argument value
#define ZPM_ADR_ARG    0x02     // address argument
#define ZPM_NDX_ARG    0x03     // index argument
#define ZPM_ARGT_BITS 3
struct zpmop {
    unsigned int code  : 8;
    unsigned int reg1  : 4;     // argument #1 register ID
    unsigned int reg2  : 4;     // argument #2 register ID
    unsigned int argt  : 6;     // argument types
    unsigned int argsz : 2;     // argument size is 8 << argsz
    unsigned int imm8  : 8;     // immediate argument such as shift count
    zpmreg       imm[EMPTY];    // possible immediate argument
};

/* predefined I/O ports */
#define ZPM_STDIN_PORT  0       // keyboard input
#define ZPM_STDOUT_PORT 1       // console or framebuffer output
#define ZPM_STDERR_PORT 2       // console or framebuffer output
#define ZPM_MOUSE_PORT  3       // mouse input
#define ZPM_RTC_PORT    4       // real-time clock
#define ZPM_TMR_PORT    5       // timer interrupt configuration

/* framebuffer graphics interface */
#define ZPM_FB_BASE     (3UL * 1024 * 1024 * 1024)      // base address

/* operation function prototypes */

#endif /* __ZPM_ZPM_H__ */

