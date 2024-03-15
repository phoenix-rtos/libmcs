/* SPDX-License-Identifier: SunMicrosystems */
/* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved. */
/* Conversion to float by Ian Lance Taylor, Cygnus Support, ian@cygnus.com. */

#include <math.h>
#include "../common/tools.h"

static const float one = 1.0f, two = 2.0f;

float tanhf(float x)
{
#ifdef __LIBMCS_FPU_DAZ
    x *= __volatile_onef;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    float t, z;
    uint32_t jx, ix;

    GET_FLOAT_WORD(jx, x);
    ix = jx & 0x7fffffffU;

    /* x is INF or NaN */
    if (!FLT_UWORD_IS_FINITE(ix)) {
        if (FLT_UWORD_IS_NAN(ix)) {         /* tanh(NaN) = NaN */
            return x + x;
        } else if ((int32_t)jx >= 0) {
            return one;                     /* tanh(+inf)=+1 */
        } else {
            return -one;                    /* tanh(-inf)=-1 */
        }
    }

    /* |x| < 22 */
    if (ix < 0x41b00000U) {                 /* |x|<22 */
        if (ix < 0x24000000U) {             /* |x|<2**-55 */
            if (FLT_UWORD_IS_ZERO(ix)) {    /* return x inexact except 0 */
                return x;
            } else {
                return __raise_inexactf(x);
            }
        }

        if (ix >= 0x3f800000U) {     /* |x|>=1  */
            t = expm1f(two * fabsf(x));
            z = one - two / (t + two);
        } else {
            t = expm1f(-two * fabsf(x));
            z = -t / (t + two);
        }

    /* |x| > 22, return +-1 */
    } else {
        z = __raise_inexactf(one);              /* raised inexact flag */
    }

    return ((int32_t)jx >= 0) ? z : -z;
}

#ifdef __LIBMCS_DOUBLE_IS_32BITS

double tanh(double x)
{
    return (double) tanhf((float) x);
}

#endif /* #ifdef __LIBMCS_DOUBLE_IS_32BITS */
