#ifndef __ZERO_IA32_TYPES_H__
#define __ZERO_IA32_TYPES_H__

#include <kern/conf.h>
#include <stdint.h>
#include <signal.h>
#include <zero/cdefs.h>
#include <zero/param.h>
#include <zero/x86/types.h>

/* C call frame - 8 bytes */
struct m_stkframe {
    /* automatic variables go here */
    int32_t fp;         // caller frame pointer
    int32_t pc;         // return address
    /* call parameters on stack go here in 'reverse order' */
};

/* general purpose registers - 32 bytes; in PUSHA order */
struct m_genregs {
    int32_t edi;
    int32_t esi;
    int32_t ebp;
    int32_t esp;
    int32_t ebx;
    int32_t edx;
    int32_t ecx;
    int32_t eax;
};

/* data segment registers - 16 bytes */
struct m_segregs {
    int32_t gs;         // kernel per-CPU segment
    int32_t fs;         // thread-local storage
    int32_t es;         // data segment
    int32_t ds;         // data segment
};

/* floating-point state */
#define X86_FXSR_MAGIC 0x0000
struct m_fpstate {
    int32_t             _cw;
    int32_t             _sw;
    int32_t             _tag;
    int32_t             _ipofs;
    int32_t             _cs;
    int32_t             _dataofs;
    int32_t             _ds;
    struct m_fpreg      _st[8];
    int16_t             _status;
    int16_t             _magic;  // 0xffff means regular FPU data only
    /* FXSR FPU environment */
    int32_t             _fxsrenv[6];
    int32_t             _mxcsr;
    int32_t             _res;
    struct m_fpxreg     _fxsr[8];
    struct m_xmmreg     _xmm[8];
    int32_t             _pad1[44];
    union {
        int32_t         _pad2[12];
        struct m_fpdata _swres;
    } u;
};

/* interrupt return frame for iret - 20 bytes */
struct m_trapframe {
    int32_t eip;	// old instruction pointer
    int16_t cs;		// code segment selector
    int16_t pad1;	// pad to 32-bit boundary
    int32_t eflags;	// machine status word
    /* present in case of privilege transition */
    int32_t uesp;	// user stack pointer
    int16_t uss;	// user stack segment selector
    int16_t pad2;	// pad to 32-bit boundary
};

/* task state segment */
struct m_tss {
    uint16_t link, _linkhi;
    uint32_t esp0;
    uint16_t ss0, _ss0hi;
    uint32_t esp1;
    uint16_t ss1, _ss1hi;
    uint32_t esp2;
    uint16_t ss2, _ss2hi;
    uint32_t cr3;
    uint32_t eip;
    uint32_t eflags;
    uint32_t eax;
    uint32_t ecx;
    uint32_t edx;
    uint32_t ebx;
    uint32_t esp;
    uint32_t ebp;
    uint32_t esi;
    uint32_t edi;
    uint16_t es, _eshi;
    uint16_t cs, _cshi;
    uint16_t ss, _sshi;
    uint16_t ds, _dshi;
    uint16_t fs, _fshi;
    uint16_t gs, _gshi;
    uint16_t ldt, _ldthi;
    uint16_t trace;
    uint16_t iomapofs;
    long     iomapflg;  // indicates presence of an 8K iomap field
    uint8_t  iomap[EMPTY] ALIGNED(PAGESIZE);
};

#if (PERTHRSTACKS)

struct m_tcb {
    int32_t             pdbr;
    struct m_segregs    segregs;
    struct m_genregs    genregs;
    struct m_trapframe  trapframe;
};

#else

/* thread control block */
struct m_tcb {
    struct m_segregs   segregs;         // data-segment registers
    struct m_genregs   genregs;         // general-purpose registers
    int32_t            pdbr;            // page-directory base register (%cr3)
    int32_t            trapnum;         // # of trap
    int32_t            err;             // error code for trap or zero
    struct m_trapframe trapframe;       // return frame for iret
};

#endif

struct m_ctx {
    int              onstk;     // non-zero if on signal-stack
    sigset_t         oldmask;   // signal mask to restore
    struct m_tcb     tcb;       // task control block (machine context)
    int32_t          cr2;       // page-fault virtual address
    int              fpflg;     // <kern/unit/ia32/task.h>
    struct m_fpstate fpstate;   // FPU state
};

struct m_task {
    int32_t          flg;       //  4 bytes @ 0; task flags
    struct m_tcb     tcb;       // 80 bytes @ 4; task control block
    int32_t          trapesp;   //  4 bytes @ 84; stack-pointer for rescheduling
    int8_t           _res[40];  // 40 bytes @ 88, pad to cacheline-boundary
    struct m_fpstate fpstate;   //  X bytes @ 128; FPU state
};

#endif /* __ZERO_IA32_TYPES_H__ */

