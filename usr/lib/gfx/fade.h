#ifndef __GFX_FADE_H__
#define __GFX_FADE_H__

#include <gfx/rgb.h>

/* basic version */

#define gfxfadein(src, dest, val) gfxfadein1(src, dest, val)
#define gfxfadeout(src, dest, val) gfxfadeout1(src, dest, val)

#define gfxfadein1(src, dest, val)                                      \
    do {                                                                \
        argb32_t _rval;                                                 \
        argb32_t _gval;                                                 \
        argb32_t _bval;                                                 \
        float    _ftor;                                                 \
                                                                        \
        _ftor = (float)val / 0xff;                                      \
        _rval = (argb32_t)(_ftor * gfxredval(src) + _ftor * gfxredval(dest)); \
        _gval = (argb32_t)(_ftor * gfxgreenval(src) + _ftor * gfxgreenval(dest)); \
        _bval = (argb32_t)(_ftor * gfxblueval(src) + _ftor * gfxblueval(dest)); \
        gfxmkpix_p(dest, 0, _rval, _gval, _bval);                       \
    } while (FALSE)

#define gfxfadeout1(src, dest, val)                                     \
    do {                                                                \
        argb32_t _rval;                                                 \
        argb32_t _gval;                                                 \
        argb32_t _bval;                                                 \
        float    _ftor;                                                 \
                                                                        \
        _ftor = (float)(0xff - val) / 0xff;                             \
        _rval = (argb32_t)(_ftor * gfxredval(src));                     \
        _gval = (argb32_t)(_ftor * gfxgreenval(src));                   \
        _bval = (argb32_t)(_ftor * gfxblueval(src));                    \
        gfxmkpix_p(dest, 0, _rval, _gval, _bval);                       \
    } while (FALSE)

/* use lookup table to eliminate division _and_ multiplication + typecasts */

/*
 * initialise lookup table
 * u8p64k points to 65536 uint8_t values like in
 * uint8_t fadetab[256][256];
 */

#define gfxinitfade1(u8p64k)                                            \
    do {                                                                \
        long  _l, _m;                                                   \
        float _f;                                                       \
        for (_l = 0 ; _l <= 0xff ; _l++) {                              \
            f = (float)val / 0xff;                                      \
            for (_m = 0 ; _m <= 0xff ; _m++) {                          \
                (u8p64k)[_l][_m] = (uint8_t)(_f * _m);                  \
            }                                                           \
        }                                                               \
    } while (0)

#define gfxfadein2(src, dest, val, tab)                                 \
    do {                                                                \
        _rval = (tab)[val][redval(src)];                                \
        _gval = (tab)[val][greenval(src)];                              \
        _bval = (tab)[val][blueval(src)];                               \
        gfxmkpix(dest, 0, _rval, _gval, _bval);                         \
    } while (FALSE)

#define gfxfadeout2(src, dest, val)                                     \
    do {                                                                \
        val = 0xff - val;                                               \
        _rval = (tab)[val][redval(src)];                                \
        _gval = (tab)[val][greenval(src)];                              \
        _bval = (tab)[val][blueval(src)];                               \
        gfxmkpix(dest, 0, _rval, _gval, _bval);                         \
    } while (FALSE)

#endif /* __GFX_FADE_H__ */

