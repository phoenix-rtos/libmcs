/* SPDX-License-Identifier: SunMicrosystems */
/* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved. */
/* Conversion to float by Ian Lance Taylor, Cygnus Support, ian@cygnus.com. */

#include <math.h>
#include "../common/tools.h"

float ceilf(float x)
{
#ifdef __LIBMCS_FPU_DAZ
    x *= __volatile_onef;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    int32_t _j0;
    uint32_t _i0, i, ix;
    GET_FLOAT_WORD(_i0, x);
    ix = (_i0 & 0x7fffffffU);
    _j0 = (int32_t)(ix >> 23U) - 0x7f;

    if (_j0 < 23) {
        if (_j0 < 0) {  /* raise inexact if x != 0 */
            if (FLT_UWORD_IS_ZERO(ix)) {
                return x;
            }

            (void) __raise_inexactf(x); /* raise inexact flag */

            if ((int32_t)_i0 < 0) {
                _i0 = 0x80000000U;
            } else {
                _i0 = 0x3f800000;
            }
        } else {
            i = 0x007fffffU >> (uint32_t)_j0;

            if ((_i0 & i) == 0U) {
                return x;    /* x is integral */
            }

            (void) __raise_inexactf(x); /* raise inexact flag */

            if ((int32_t)_i0 > 0) {
                _i0 += 0x00800000U >> (uint32_t)_j0;
            }

            _i0 &= (~i);
        }
    } else {
        if (!FLT_UWORD_IS_FINITE(ix)) {
            return x + x;    /* inf or NaN */
        } else {
            return x;    /* x is integral */
        }
    }

    SET_FLOAT_WORD(x, _i0);
    return x;
}

#ifdef __LIBMCS_DOUBLE_IS_32BITS

double ceil(double x)
{
    return (double) ceilf((float) x);
}

#endif /* #ifdef __LIBMCS_DOUBLE_IS_32BITS */
