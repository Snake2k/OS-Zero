#ifndef __ZPU_CONF_H__
#define __ZPU_CONF_H__

#define ZPUIREGSIZE 4   // integer register size in bytes        
#define ZPURAT      1   // enable or disable rational operations
#define ZPUSIMD     1   // enable or disable SIMD operations

#define ZPUCORESIZE (512 * 1024 * 1024) // emulator memory size

#endif /* __ZPU_CONF_H__ */

