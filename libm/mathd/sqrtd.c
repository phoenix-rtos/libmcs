/* SPDX-License-Identifier: SunMicrosystems */
/* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved. */

/**
 *
 * This family of functions implements the square root of :math:`x`.
 *
 * Synopsis
 * ========
 *
 * .. code-block:: c
 *
 *     #include <math.h>
 *     float sqrtf(float x);
 *     double sqrt(double x);
 *     long double sqrtl(long double x);
 *
 * Description
 * ===========
 *
 * ``sqrt`` computes the square root of the input value.
 *
 * Mathematical Function
 * =====================
 *
 * .. math::
 *
 *    sqrt(x) \approx \sqrt{x}
 *
 * Returns
 * =======
 *
 * ``sqrt`` returns the square root of the input value.
 *
 * Exceptions
 * ==========
 *
 * Raise ``invalid operation`` exception when :math:`x` is negative.
 *
 * Output map
 * ==========
 *
 * +---------------------+--------------+------------------+--------------+--------------+------------------+--------------+--------------+
 * | **x**               | :math:`-Inf` | :math:`<0`       | :math:`-0`   | :math:`+0`   | :math:`>0`       | :math:`+Inf` | :math:`NaN`  |
 * +=====================+==============+==================+==============+==============+==================+==============+==============+
 * | **sqrt(x)**         | :math:`qNaN` | :math:`qNaN`     | :math:`x`                   | :math:`\sqrt{x}` | :math:`+Inf` | :math:`qNaN` |
 * +---------------------+--------------+------------------+--------------+--------------+------------------+--------------+--------------+
 *
 *///

#include <math.h>
#include "../common/tools.h"

#ifndef __LIBMCS_DOUBLE_IS_32BITS

double sqrt(double x)
{
#ifdef __LIBMCS_FPU_DAZ
    x *= __volatile_one;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    double z;
    uint32_t sign = 0x80000000U;
    uint32_t r, t1, s1, ix1, q1, ix0, s0, q, m, t, i;

    EXTRACT_WORDS(ix0, ix1, x);

    /* take care of Inf and NaN */
    if ((ix0 & 0x7ff00000U) == 0x7ff00000U) {
        if (DBL_WORDS_IS_NAN(ix0, ix1)) {   /* sqrt(NaN)=NaN */
            return x + x;
        } else if ((int32_t)ix0 > 0) {   /* sqrt(+inf)=+inf */
            return x;
        } else {                /* sqrt(-inf)=sNaN */
            return __raise_invalid();
        }
    }

    /* take care of zero and negative values */
    if ((int32_t)ix0 <= 0) {
        if (((ix0 & (~sign)) | ix1) == 0U) {
            return x;    /* sqrt(+-0) = +-0 */
        } else if ((int32_t)ix0 < 0) {
            return __raise_invalid();    /* sqrt(-ve) = sNaN */
        } else {
            /* No action required */
        }
    }

    /* normalize x */
    m = (ix0 >> 20U);

    if (m == 0U) {             /* subnormal x */
        while (ix0 == 0U) {
            m -= 21U;
            ix0 |= (ix1 >> 11U);
            ix1 <<= 21U;
        }

        for (i = 0U; (ix0 & 0x00100000U) == 0U; i++) {
            ix0 <<= 1U;
        }

        m -= i - 1U;
        ix0 |= (ix1 >> (32U - i));
        ix1 <<= i;
    }

    m -= 1023U;    /* unbias exponent */
    ix0 = (ix0 & 0x000fffffU) | 0x00100000U;

    if (0U < (m & 1U)) { /* odd m, double x to make it even */
        ix0 += ix0 + ((ix1 & sign) >> 31U);
        ix1 += ix1;
    }

    m >>= 1U;    /* m = [m/2] */

    /* generate sqrt(x) bit by bit */
    ix0 += ix0 + ((ix1 & sign) >> 31U);
    ix1 += ix1;
    q = q1 = s0 = s1 = 0U;    /* [q,q1] = sqrt(x) */
    r = 0x00200000U;          /* r = moving bit from right to left */

    while (r != 0U) {
        t = s0 + r;

        if (t <= ix0) {
            s0   = t + r;
            ix0 -= t;
            q   += r;
        }

        ix0 += ix0 + ((ix1 & sign) >> 31U);
        ix1 += ix1;
        r >>= 1U;
    }

    r = sign;

    while (r != 0U) {
        t1 = s1 + r;
        t  = s0;

        if ((t < ix0) || ((t == ix0) && (t1 <= ix1))) {
            s1  = t1 + r;

            if (((t1 & sign) == sign) && (s1 & sign) == 0U) {
                s0 += 1U;
            }

            ix0 -= t;

            if (ix1 < t1) {
                ix0 -= 1U;
            }

            ix1 -= t1;
            q1  += r;
        }

        ix0 += ix0 + ((ix1 & sign) >> 31U);
        ix1 += ix1;
        r >>= 1U;
    }

    /* use floating add to find out rounding direction */
    if ((ix0 | ix1) != 0U) {
        (void) __raise_inexact(x);
        if (q1 == 0xffffffffU) {
            q1 = 0U;
            q += 1U;
        } else {
            q1 += (q1 & 1U);
        }
    }

    ix0 = (q >> 1U) + 0x3fe00000U;
    ix1 =  q1 >> 1U;

    if ((q & 1U) != 0U) {
        ix1 |= sign;
    }

    ix0 += (m << 20U);
    INSERT_WORDS(z, ix0, ix1);
    return z;
}

#ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS

long double sqrtl(long double x)
{
    return (long double) sqrt((double) x);
}

#endif /* #ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS */
#endif /* #ifndef __LIBMCS_DOUBLE_IS_32BITS */
