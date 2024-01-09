/* SPDX-License-Identifier: SunMicrosystems */
/* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved. */
/* Conversion to float by Ian Lance Taylor, Cygnus Support, ian@cygnus.com. */

/*
 * fmodf(x,y)
 * Return x mod y in exact arithmetic
 * Method: shift and subtract
 */

#include <math.h>
#include "../common/tools.h"

static const float Zero[] = {0.0f, -0.0f,};

float fmodf(float x, float y)
{
#ifdef __LIBMCS_FPU_DAZ
    x *= __volatile_onef;
    y *= __volatile_onef;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    int32_t n, ix, iy;
    uint32_t hx, hy, hz, sx, i;

    GET_FLOAT_WORD(hx, x);
    GET_FLOAT_WORD(hy, y);
    sx = hx & 0x80000000U;      /* sign of x */
    hx ^= sx;                   /* |x| */
    hy &= 0x7fffffffU;          /* |y| */

    /* purge off exception values */
    if (!FLT_UWORD_IS_FINITE(hx) || !FLT_UWORD_IS_FINITE(hy)) {     /* x or y is +-Inf/NaN */
        if (FLT_UWORD_IS_INFINITE(hx)) {                            /* x is +-Inf */
            return __raise_invalidf();
        } else if (FLT_UWORD_IS_NAN(hx) || FLT_UWORD_IS_NAN(hy)) {  /* x or y is NaN */
            return x + y;
        } else {
            /* No action required */
        }
    } else if (FLT_UWORD_IS_ZERO(hy)) {                             /* y is +-0 */
        return __raise_invalidf();
    } else {
        /* No action required */
    }

    if (hx < hy) {
        return x;    /* |x|<|y| return x */
    }

    if (hx == hy) {
        return Zero[sx >> 31U];    /* |x|=|y| return x*0*/
    }

    /* Note: y cannot be zero if we reach here. */

    /* determine ix = ilogb(x) */
    if (FLT_UWORD_IS_SUBNORMAL(hx)) {   /* subnormal x */
        for (ix = -126, i = hx << 8U; (int32_t)i > 0; i <<= 1U) {
            ix -= 1;
        }
    } else {
        ix = (int32_t)(hx >> 23U) - 127;
    }

    /* determine iy = ilogb(y) */
    if (FLT_UWORD_IS_SUBNORMAL(hy)) {   /* subnormal y */
        for (iy = -126, i = hy << 8U; (int32_t)i >= 0; i <<= 1U) {
            iy -= 1;
        }
    } else {
        iy = (int32_t)(hy >> 23U) - 127;
    }

    /* set up {hx,lx}, {hy,ly} and align y to x */
    if (ix >= -126) {
        hx = 0x00800000U | (0x007fffffU & hx);
    } else {      /* subnormal x, shift x to normal */
        n = -126 - ix;
        hx = hx << (uint32_t)n;
    }

    if (iy >= -126) {
        hy = 0x00800000U | (0x007fffffU & hy);
    } else {      /* subnormal y, shift y to normal */
        n = -126 - iy;
        hy = hy << (uint32_t)n;
    }

    /* fix point fmod */
    n = ix - iy;

    while (n-- > 0) {
        hz = hx - hy;

        if ((int32_t)hz < 0) {
            hx = hx + hx;
        } else {
            if (hz == 0U) {    /* return sign(x)*0 */
                return Zero[sx >> 31U];
            }

            hx = hz + hz;
        }
    }

    hz = hx - hy;

    if ((int32_t)hz >= 0) {
        hx = hz;
    }

    /* convert back to floating value and restore the sign */
    if (hx == 0U) {        /* return sign(x)*0 */
        return Zero[sx >> 31U];
    }

    while (hx < 0x00800000U) {     /* normalize x */
        hx = hx + hx;
        iy -= 1;
    }

    if (iy >= -126) {      /* normalize output */
        hx = (hx - 0x00800000U) | ((uint32_t)(iy + 127) << 23U);
        SET_FLOAT_WORD(x, hx | sx);
    } else {        /* subnormal output */
        n = -126 - iy;
        hx >>= (uint32_t)n;
        SET_FLOAT_WORD(x, hx | sx);
#ifdef __LIBMCS_FPU_DAZ
        x *= __volatile_onef;
#endif /* defined(__LIBMCS_FPU_DAZ) */
    }

    return x;        /* exact output */
}

#ifdef __LIBMCS_DOUBLE_IS_32BITS

double fmod(double x, double y)
{
    return (double) fmodf((float) x, (float) y);
}

#endif /* #ifdef __LIBMCS_DOUBLE_IS_32BITS */
