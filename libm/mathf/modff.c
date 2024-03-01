/* SPDX-License-Identifier: SunMicrosystems */
/* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved. */
/* Conversion to float by Ian Lance Taylor, Cygnus Support, ian@cygnus.com. */

#include <assert.h>
#include <math.h>
#include "../common/tools.h"

float modff(float x, float *iptr)
{
#ifdef __LIBMCS_FPU_DAZ
    x *= __volatile_onef;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    float _xi = 0.0f;
    int32_t _j0;
    uint32_t i, _i0;

    assert(iptr != (void*)0U);
    if (iptr == (void*)0U) {
        iptr = &_xi;
    }

    GET_FLOAT_WORD(_i0, x);
    _j0 = (int32_t)((_i0 >> 23U) & 0xffU) - 0x7f; /* exponent of x */
    
    if (_j0 < 23) {         /* integer part in x */
        if (_j0 < 0) {         /* |x|<1 */
            SET_FLOAT_WORD(*iptr, _i0 & 0x80000000U); /* *iptr = +-0 */
            return x;
        } else {
            i = 0x007fffffU >> (uint32_t)_j0;

            if ((_i0 & i) == 0U) {       /* x is integral */
                *iptr = x;
                SET_FLOAT_WORD(x, _i0 & 0x80000000U); /* return +-0 */
                return x;
            } else {
                SET_FLOAT_WORD(*iptr, _i0 & (~i));
                return x - *iptr;
            }
        }
    } else {            /* no fraction part */
        *iptr = x;

        if (isnan(x)) {
            return *iptr = x + x;    /* x is NaN, return NaN */
        }

        SET_FLOAT_WORD(x, _i0 & 0x80000000U); /* return +-0 */
        return x;
    }
}

#ifdef __LIBMCS_DOUBLE_IS_32BITS

double modf(double x, double *iptr)
{
    return (double) modff((float) x, (float *) iptr);
}

#endif /* #ifdef __LIBMCS_DOUBLE_IS_32BITS */
