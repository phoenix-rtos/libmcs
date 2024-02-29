/* SPDX-License-Identifier: SunMicrosystems */
/* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved. */
/* Conversion to float by Ian Lance Taylor, Cygnus Support, ian@cygnus.com. */

#include <math.h>
#include "../common/tools.h"

float nextafterf(float x, float y)
{
#ifdef __LIBMCS_FPU_DAZ
    x *= __volatile_onef;
    y *= __volatile_onef;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    uint32_t hx, hy, ix, iy;

    GET_FLOAT_WORD(hx, x);
    GET_FLOAT_WORD(hy, y);
    ix = hx & 0x7fffffffU;      /* |x| */
    iy = hy & 0x7fffffffU;      /* |y| */

    if (FLT_UWORD_IS_NAN(ix) || FLT_UWORD_IS_NAN(iy)) {
        return x + y;
    } else if (hx == hy) {
        return x;                      /* x=y, return x */
    } else if (ix == 0U) {              /* x == 0 */
        if (ix == iy) {
            return x;                  /* x=y, return x */
        }
#ifdef __LIBMCS_FPU_DAZ
        SET_FLOAT_WORD(x, (hy & 0x80000000U) | 0x00800000U);    /* return +-minnormal */
#else
        SET_FLOAT_WORD(x, (hy & 0x80000000U) | 1U);             /* return +-minsubnormal */
        (void) __raise_underflowf(x);
#endif /* defined(__LIBMCS_FPU_DAZ) */
        return x;
    } else if ((int32_t)hx >= 0) {              /* x > 0 */
        if ((int32_t)hx > (int32_t)hy) {                 /* x > y, x -= ulp */
            hx -= 1U;
        } else {                       /* x < y, x += ulp */
            hx += 1U;
        }
    } else {                           /* x < 0 */
        if ((int32_t)hy >= 0 || (int32_t)hx > (int32_t)hy) {      /* x < y, x -= ulp */
            hx -= 1U;
        } else {                       /* x > y, x += ulp */
            hx += 1U;
        }
    }

    hy = hx & 0x7f800000U;

    if (hy > FLT_UWORD_MAX) {
        return __raise_overflowf(x);  /* overflow if x is finite */
    }

    if (hy < 0x00800000U) {            /* underflow */
#ifdef __LIBMCS_FPU_DAZ
        SET_FLOAT_WORD(x, hx & 0x80000000U);    /* return +-0.0 */
        return x;
#else
        (void) __raise_underflowf(x);
#endif /* defined(__LIBMCS_FPU_DAZ) */
    }

    SET_FLOAT_WORD(x, hx);
    return x;
}

#ifdef __LIBMCS_DOUBLE_IS_32BITS

double nextafter(double x, double y)
{
    return (double) nextafterf((float) x, (float) y);
}

#endif /* #ifdef __LIBMCS_DOUBLE_IS_32BITS */
