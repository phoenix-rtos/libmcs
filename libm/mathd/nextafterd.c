/* SPDX-License-Identifier: SunMicrosystems */
/* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved. */

/**
 *
 * This family of functions computes the next floating-point value after
 * :math:`x` in direction of :math:`y`.
 *
 * Synopsis
 * ========
 *
 * .. code-block:: c
 *
 *     #include <math.h>
 *     float nextafterf(float x, float y);
 *     double nextafter(double x, double y);
 *     long double nextafterl(long double x, long double y);
 *
 * Description
 * ===========
 *
 * ``nextafter`` computes the next floating-point value after :math:`x` in direction of :math:`y`.
 *
 * Mathematical Function
 * =====================
 *
 * .. math::
 *
 *    nextafter(x, y) = \left\{\begin{array}{ll} y, & x = y \\ \text{min} \{z | z \in \mathbb{F} \wedge z > x\}, & x < y \\ \text{max} \{z | z \in \mathbb{F} \wedge z < x\}, & otherwise \end{array}\right.
 *
 * Returns
 * =======
 *
 * ``nextafter`` returns the the next floating-point value after :math:`x` in
 * direction of :math:`y`.
 *
 * Exceptions
 * ==========
 *
 * Raise ``overflow`` exception when the magnitude of :math:`x` is the largest
 * finite value representable in the type and :math:`y` is infinite with the
 * same sign as :math:`x`.
 *
 * .. May raise ``underflow`` exception.
 *
 * Output map
 * ==========
 *
 * +--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+
 * | nextafter(x,y)           | x                                                                                                                                                                                          |
 * +--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+
 * | y                        | :math:`-Inf`             | :math:`<0`               | :math:`-0`               | :math:`+0`               | :math:`>0`               | :math:`+Inf`             | :math:`NaN`              |
 * +==========================+==========================+==========================+==========================+==========================+==========================+==========================+==========================+
 * | :math:`-Inf`             | :math:`-Inf`             | :math:`nextafter(x, y)`  | max :math:`\mathbb{S}^{-}`                          | :math:`nextafter(x, y)`  | :math:`nextafter(x, y)`  | :math:`qNaN`             |
 * +--------------------------+--------------------------+                          +                                                     +                          +                          +                          +
 * | :math:`<0`               | :math:`nextafter(x, y)`  |                          |                                                     |                          |                          |                          |
 * +--------------------------+                          +                          +--------------------------+--------------------------+                          +                          +                          +
 * | :math:`-0`               |                          |                          | :math:`y`                                           |                          |                          |                          |
 * +--------------------------+                          +                          +                                                     +                          +                          +                          +
 * | :math:`+0`               |                          |                          |                                                     |                          |                          |                          |
 * +--------------------------+                          +                          +--------------------------+--------------------------+                          +                          +                          +
 * | :math:`>0`               |                          |                          | min :math:`\mathbb{S}^{+}`                          |                          |                          |                          |
 * +--------------------------+                          +                          +                                                     +                          +--------------------------+                          +
 * | :math:`+Inf`             |                          |                          |                                                     |                          | :math:`+Inf`             |                          |
 * +--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+                          +
 * | :math:`NaN`              | :math:`qNaN`                                                                                                                                                    |                          |
 * +--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+--------------------------+
 *
 */

#include <math.h>
#include "../common/tools.h"

#ifndef __LIBMCS_DOUBLE_IS_32BITS

double nextafter(double x, double y)
{
#ifdef __LIBMCS_FPU_DAZ
    x *= __volatile_one;
    y *= __volatile_one;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    uint32_t lx, ly, hx, hy, ix;

    EXTRACT_WORDS(hx, lx, x);
    EXTRACT_WORDS(hy, ly, y);
    ix = hx & 0x7fffffffU;               /* |x| */

    if (isnan(x) || isnan(y)) {         /* x or y is nan */
        return x + y;
    } else if (hx == hy && lx == ly) {
        return y;                       /* x=y, return x */
    } else if ((ix | lx) == 0U) {        /* x == 0 */
        if (ix == (hy & 0x7fffffffU) && ly == 0U) {
            return y;                   /* x=y, return x */
        }
#ifdef __LIBMCS_FPU_DAZ
        INSERT_WORDS(x, (hy & 0x80000000U) | 0x00100000U, 0U);  /* return +-minnormal */
#else
        INSERT_WORDS(x, hy & 0x80000000U, 1U);                  /* return +-minsubnormal */
        (void) __raise_underflow(x);
#endif /* defined(__LIBMCS_FPU_DAZ) */
        return x;
    } else if ((int32_t)hx >= 0) {               /* x > 0 */
        if ((int32_t)hx > (int32_t)hy || ((hx == hy) && (lx > ly))) {             /* x > y, x -= ulp */
            if (lx == 0U) {
                hx -= 1U;
            }

            lx -= 1U;
        } else {                        /* x < y, x += ulp */
            lx += 1U;

            if (lx == 0U) {
                hx += 1U;
            }
        }
    } else {                            /* x < 0 */
        if ((int32_t)hy >= 0 || (int32_t)hx > (int32_t)hy || ((hx == hy) && (lx > ly))) {  /* x < y, x -= ulp */
            if (lx == 0U) {
                hx -= 1U;
            }

            lx -= 1U;
        } else {                        /* x > y, x += ulp */
            lx += 1U;

            if (lx == 0U) {
                hx += 1U;
            }
        }
    }

    hy = hx & 0x7ff00000U;

    if (hy >= 0x7ff00000U) {
        return __raise_overflow(x);     /* overflow if x is finite */
    }

    if (hy < 0x00100000U) {              /* underflow */
#ifdef __LIBMCS_FPU_DAZ
        INSERT_WORDS(x, hx & 0x80000000U, 0U);  /* return +-0.0 */
        return x;
#else
        (void) __raise_underflow(x);
#endif /* defined(__LIBMCS_FPU_DAZ) */
    }

    INSERT_WORDS(x, hx, lx);
    return x;
}

#ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS

long double nextafterl(long double x, long double y)
{
    return (long double) nextafter((double) x, (double) y);
}

#endif /* #ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS */
#endif /* #ifndef __LIBMCS_DOUBLE_IS_32BITS */
