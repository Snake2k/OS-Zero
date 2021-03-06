#ifndef __VT_CONF_H__
#define __VT_CONF_H__

#define TERMXORG        1
#define TERMNSCREEN     2
#define TERMNSCREENVT   4

#define VTDEBUGESC      1

#define VTDEFFONT       "fixed"
#define VTDEFNROW       24
#define VTDEFNCOL       80

#define VTXORG          1
#define VT_POSIX_OPENPT 1
#define VT_GETPT        0
#define VT_DEV_PTMX     0
#define VT_PTMX_DEVICE  "/dev/ptmx"
#if (VT_POSIX_OPENPT)
#define _XOPEN_SOURCE   600
#endif
#if (VT_GETPT)
#define _GNU_SOURCE     1
#endif

#endif /* __VT_CONF_H__ */

