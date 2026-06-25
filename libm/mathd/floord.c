/* SPDX-License-Identifier: SunMicrosystems */
/* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved. */

/**
 *
 * This family of functions implements rounding towards negative infinity of
 * :math:`x`.
 *
 * Synopsis
 * ========
 *
 * .. code-block:: c
 *
 *     #include <math.h>
 *     float floorf(float x);
 *     double floor(double x);
 *     long double floorl(long double x);
 *
 * Description
 * ===========
 *
 * ``floor`` computes the input value rounded towards negative infinity.
 *
 * Mathematical Function
 * =====================
 *
 * .. math::
 *
 *    floor(x) = \lfloor x \rfloor
 *
 * Returns
 * =======
 *
 * ``floor`` returns the input value rounded towards negative infinity.
 *
 * Exceptions
 * ==========
 *
 * Does not raise exceptions.
 *
 * Output map
 * ==========
 *
 * +--------------+--------------+---------------------------+--------------+------------+---------------------------+--------------+--------------+
 * | **x**        | :math:`-Inf` | :math:`<0`                | :math:`-0`   | :math:`+0` | :math:`>0`                | :math:`+Inf` | :math:`NaN`  |
 * +==============+==============+===========================+==============+============+===========================+==============+==============+
 * | **floor(x)** | :math:`-Inf` | :math:`\lfloor x \rfloor` | :math:`x`                 | :math:`\lfloor x \rfloor` | :math:`+Inf` | :math:`qNaN` |
 * +--------------+--------------+---------------------------+--------------+------------+---------------------------+--------------+--------------+
 *
 */

#include <math.h>
#include "../common/tools.h"

#ifndef __LIBMCS_DOUBLE_IS_32BITS

double floor(double x)
{
#ifdef __LIBMCS_FPU_DAZ
    x *= __volatile_one;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    uint32_t _i0, _i1, _j0, i, j;

    EXTRACT_WORDS(_i0, _i1, x);
    _j0 = ((_i0 >> 20U) & 0x000007ffU) - 0x000003ffU;

    if ((int32_t)_j0 < 20) {
        if ((int32_t)_j0 < 0) {  /* raise inexact if x != 0 */
            if (((_i0 & 0x7fffffffU) | _i1) == 0U) {
                return x;
            }

            (void) __raise_inexact(x);

            if ((int32_t)_i0 >= 0) {
                _i0 = _i1 = 0U;
            } else {
                _i0 = 0xbff00000U;
                _i1 = 0U;
            }
        } else {
            i = (0x000fffffU) >> _j0;

            if (((_i0 & i) | _i1) == 0U) {
                return x;    /* x is integral */
            }

            (void) __raise_inexact(x);

            if ((int32_t)_i0 < 0) {
                _i0 += (0x00100000U) >> _j0;
            }

            _i0 &= (~i);
            _i1 = 0U;
        }
    } else if ((int32_t)_j0 > 51) {
        if (_j0 == 0x00000400U) {
            return x + x;    /* inf or NaN */
        } else {
            return x;    /* x is integral */
        }
    } else {
        i = 0xffffffffU >> (_j0 - 20U);

        if ((_i1 & i) == 0U) {
            return x;    /* x is integral */
        }

        (void) __raise_inexact(x);

        if ((int32_t)_i0 < 0) {
            if (_j0 == 20U) {
                _i0 += 1U;
            } else {
                j = _i1 + ((uint32_t)1U << (52U - _j0));

                if (j < _i1) {
                    _i0 += 1U ;    /* got a carry */
                }

                _i1 = j;
            }
        }

        _i1 &= (~i);
    }

    INSERT_WORDS(x, _i0, _i1);
    return x;
}

#ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS

long double floorl(long double x)
{
    return (long double) floor((double) x);
}

#endif /* #ifdef __LIBMCS_LONG_DOUBLE_IS_64BITS */
#endif /* #ifndef __LIBMCS_DOUBLE_IS_32BITS */
