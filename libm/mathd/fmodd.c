/* SPDX-License-Identifier: SunMicrosystems */
/* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved. */

/**
 *
 * This family of functions implements the floating-point remainder of
 * :math:`x` divided by :math:`y`.
 *
 * Synopsis
 * ========
 *
 * .. code-block:: c
 *
 *     #include <math.h>
 *     float fmodf(float x, float y);
 *     double fmod(double x, double y);
 *     long double fmodl(long double x, long double y);
 *
 * Description
 * ===========
 *
 * ``fmod`` computes the floating-point remainder of :math:`x` divided by
 * :math:`y`.
 *
 * The ``fmod`` and ``remainder`` procedures are rather similar, but not the
 * same, see examples:
 *
 * +----------------+----------------+----------------+----------------+
 * | x              | y              | fmod           | remainder      |
 * +================+================+================+================+
 * | :math:`+2.456` | :math:`+2.0`   | :math:`+0.456` | :math:`+0.456` |
 * +----------------+----------------+----------------+----------------+
 * | :math:`+3.456` | :math:`+2.0`   | :math:`+1.456` | :math:`-0.544` |
 * +----------------+----------------+----------------+----------------+
 * | :math:`-2.456` | :math:`+2.0`   | :math:`-0.456` | :math:`-0.456` |
 * +----------------+----------------+----------------+----------------+
 * | :math:`-3.456` | :math:`+2.0`   | :math:`-1.456` | :math:`+0.544` |
 * +----------------+----------------+----------------+----------------+
 * | :math:`+2.456` | :math:`-2.0`   | :math:`+0.456` | :math:`+0.456` |
 * +----------------+----------------+----------------+----------------+
 * | :math:`+3.456` | :math:`-2.0`   | :math:`+1.456` | :math:`-0.544` |
 * +----------------+----------------+----------------+----------------+
 * | :math:`-2.456` | :math:`-2.0`   | :math:`-0.456` | :math:`-0.456` |
 * +----------------+----------------+----------------+----------------+
 * | :math:`-3.456` | :math:`-2.0`   | :math:`-1.456` | :math:`+0.544` |
 * +----------------+----------------+----------------+----------------+
 *
 * Mathematical Function
 * =====================
 *
 * .. math::
 *
 *    fmod(x, y) = x - n \cdot y \wedge n \in \mathbb{Z} \wedge |fmod(x, y)| < |y| \wedge \frac{f(x, y)}{|f(x, y)|} = \frac{x}{|x|}
 *
 * Returns
 * =======
 *
 * ``fmod`` returns the floating-point remainder of :math:`x` divided by :math:`y`.
 *
 * Exceptions
 * ==========
 *
 * Raise ``invalid operation`` exception when :math:`x` is infinite or
 * :math:`y` is zero.
 *
 * .. May raise ``underflow`` exception.
 *
 * Output map
 * ==========
 *
 * +--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+
 * | fmod(x,y)                | x                                                                                                                                                                                          |
 * +--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+
 * | y                        | :math:`-Inf`             | :math:`<0`               | :math:`-0`               | :math:`+0`               | :math:`>0`               | :math:`+Inf`             | :math:`NaN`              |
 * +==========================+==========================+==========================+==========================+==========================+==========================+==========================+==========================+
 * | :math:`-Inf`             | :math:`qNaN`             | :math:`x`                                                                                                 | :math:`qNaN`             | :math:`qNaN`             |
 * +--------------------------+                          +--------------------------+--------------------------+--------------------------+--------------------------+                          +                          +
 * | :math:`<0`               |                          | :math:`fmod(x, y)`       | :math:`x`                                           | :math:`fmod(x, y)`       |                          |                          |
 * +--------------------------+                          +--------------------------+--------------------------+--------------------------+--------------------------+                          +                          +
 * | :math:`-0`               |                          | :math:`qNaN`                                                                                              |                          |                          |
 * +--------------------------+                          +                                                                                                           +                          +                          +
 * | :math:`+0`               |                          |                                                                                                           |                          |                          |
 * +--------------------------+                          +--------------------------+--------------------------+--------------------------+--------------------------+                          +                          +
 * | :math:`>0`               |                          | :math:`fmod(x, y)`       | :math:`x`                                           | :math:`fmod(x, y)`       |                          |                          |
 * +--------------------------+                          +--------------------------+--------------------------+--------------------------+--------------------------+                          +                          +
 * | :math:`+Inf`             |                          | :math:`x`                                                                                                 |                          |                          |
 * +--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+                          +
 * | :math:`NaN`              | :math:`qNaN`                                                                                                                                                    |                          |
 * +--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+
 *
 *///

#include <math.h>
#include "../common/tools.h"

#ifndef __LIBMCS_DOUBLE_IS_32BITS

static const double Zero[] = {0.0, -0.0,};

double fmod(double x, double y)
{
#ifdef __LIBMCS_FPU_DAZ
    x *= __volatile_one;
    y *= __volatile_one;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    int32_t n, ix, iy;
    uint32_t hx, hy, hz, lx, ly, lz, sx, i;

    EXTRACT_WORDS(hx, lx, x);
    EXTRACT_WORDS(hy, ly, y);
    sx = hx & 0x80000000U;     /* sign of x */
    hx ^= sx;                  /* |x| */
    hy &= 0x7fffffffU;         /* |y| */

    /* purge off exception values */
    if (hx >= 0x7ff00000U || hy >= 0x7ff00000U) { /* x or y is +-Inf/NaN */
        if (hx == 0x7ff00000U && lx == 0U) {      /* x is +-Inf */
            return __raise_invalid();
        } else if (isnan(x) || isnan(y)) {        /* x or y is NaN */
            return x + y;
        } else {
            /* No action required */
        }
    } else if ((hy | ly) == 0U) {                  /* y is +-0 */
        return __raise_invalid();
    } else {
        /* No action required */
    }

    if (hx <= hy) {
        if ((hx < hy) || (lx < ly)) {
            return x;    /* |x|<|y| return x */
        }

        if (lx == ly) {
            return Zero[sx >> 31U];               /* |x|=|y| return x*0*/
        }
    }

    /* determine ix = ilogb(x) */
    if (hx < 0x00100000U) { /* subnormal x */
        if (hx == 0U) {
            for (ix = -1043, i = lx; (int32_t)i > 0; i <<= 1U) {
                ix -= 1;
            }
        } else {
            for (ix = -1022, i = hx << 11U; (int32_t)i > 0; i <<= 1U) {
                ix -= 1;
            }
        }
    } else {
        ix = (int32_t)(hx >> 20U) - 1023;
    }

    /* determine iy = ilogb(y) */
    if (hy < 0x00100000U) { /* subnormal y */
        if (hy == 0U) {
            for (iy = -1043, i = ly; (int32_t)i > 0; i <<= 1U) {
                iy -= 1;
            }
        } else {
            for (iy = -1022, i = hy << 11U; (int32_t)i > 0; i <<= 1U) {
                iy -= 1;
            }
        }
    } else {
        iy = (int32_t)(hy >> 20U) - 1023;
    }

    /* set up {hx,lx}, {hy,ly} and align y to x */
    if (ix >= -1022) {
        hx = 0x00100000U | (0x000fffffU & hx);
    } else {      /* subnormal x, shift x to normal */
        n = -1022 - ix;

        if (n <= 31) {
            hx = (hx << (uint32_t)n) | (lx >> (uint32_t)(32 - n));
            lx <<= (uint32_t)n;
        } else {
            hx = lx << (uint32_t)(n - 32);
            lx = 0U;
        }
    }

    if (iy >= -1022) {
        hy = 0x00100000U | (0x000fffffU & hy);
    } else {      /* subnormal y, shift y to normal */
        n = -1022 - iy;

        if (n <= 31) {
            hy = (hy << (uint32_t)n) | (ly >> (uint32_t)(32 - n));
            ly <<= (uint32_t)n;
        } else {
            hy = ly << (uint32_t)(n - 32);
            ly = 0U;
        }
    }

    /* fix point fmod */
    n = ix - iy;

    while (n-- > 0) {
        hz = hx - hy;
        lz = lx - ly;

        if (lx < ly) {
            hz -= 1U;
        }

        if ((int32_t)hz < 0) {
            hx = hx + hx + (lx >> 31U);
            lx = lx + lx;
        } else {
            if ((hz | lz) == 0U) {  /* return sign(x)*0 */
                return Zero[sx >> 31U];
            }

            hx = hz + hz + (lz >> 31U);
            lx = lz + lz;
        }
    }

    hz = hx - hy;
    lz = lx - ly;

    if (lx < ly) {
        hz -= 1U;
    }

    if ((int32_t)hz >= 0) {
        hx = hz;
        lx = lz;
    }

    /* convert back to floating value and restore the sign */
    if ((hx | lx) == 0U) {      /* return sign(x)*0 */
        return Zero[sx >> 31U];
    }

    while (hx < 0x00100000U) {     /* normalize x */
        hx = hx + hx + (lx >> 31U);
        lx = lx + lx;
        iy -= 1;
    }

    if (iy >= -1022) {  /* normalize output */
        hx = ((hx - 0x00100000U) | ((uint32_t)(iy + 1023) << 20U));
        INSERT_WORDS(x, hx | sx, lx);
    } else {        /* subnormal output */
        n = -1022 - iy;

        if (n <= 20) {
            lx = (lx >> (uint32_t)n) | (hx << (32U - (uint32_t)n));
            hx >>= (uint32_t)n;
        } else if (n <= 31) {
            lx = (hx << (uint32_t)(32 - n)) | (lx >> (uint32_t)n);
            hx = sx;
        } else {
            lx = hx >> (uint32_t)(n - 32);
            hx = sx;
        }

        INSERT_WORDS(x, hx | sx, lx);
#ifdef __LIBMCS_FPU_DAZ
        x *= __volatile_one;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    }

    return x;        /* exact output */
}

#ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS

long double fmodl(long double x, long double y)
{
    return (long double) fmod((double) x, (double) y);
}

#endif /* #ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS */
#endif /* #ifndef __LIBMCS_DOUBLE_IS_32BITS */
