/* SPDX-License-Identifier: SunMicrosystems */
/* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved. */
/* Conversion to float by Ian Lance Taylor, Cygnus Support, ian@cygnus.com. */

#include <math.h>
#include "../common/tools.h"

float sqrtf(float x)
{
#ifdef __LIBMCS_FPU_DAZ
    x *= __volatile_onef;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    float z;
    uint32_t hx, ix, i, m, q, r, s, t;

    GET_FLOAT_WORD(hx, x);
    ix = hx & 0x7fffffffU;

    /* take care of Inf and NaN */
    if (!FLT_UWORD_IS_FINITE(ix)) {
        if (FLT_UWORD_IS_NAN(ix)) {     /* sqrt(NaN)=NaN */
            return x + x;
        } else if ((int32_t)hx > 0) {   /* sqrt(+inf)=+inf */
            return x;
        } else {                        /* sqrt(-inf)=sNaN */
            return __raise_invalidf();
        }
    }

    /* take care of zero and -ves */
    if (FLT_UWORD_IS_ZERO(ix)) {
        return x;    /* sqrt(+-0) = +-0 */
    }

    if ((int32_t)hx < 0) {
        return __raise_invalidf();    /* sqrt(-ve) = sNaN */
    }

    /* normalize x */
    m = (hx >> 23U);

    if (FLT_UWORD_IS_SUBNORMAL(hx)) {       /* subnormal x */
        for (i = 0U; (hx & 0x00800000U) == 0U; i++) {
            hx <<= 1U;
        }

        m -= i - 1U;
        ix = (int32_t)hx;
    }

    m -= 127U;    /* unbias exponent */
    hx = (hx & 0x007fffffU) | 0x00800000U;

    if ((m & 1U) > 0U) { /* odd m, double x to make it even */
        hx += hx;
    }

    m >>= 1U;    /* m = [m/2] */

    /* generate sqrt(x) bit by bit */
    hx += hx;
    q = s = 0U;            /* q = sqrt(x) */
    r = 0x01000000U;       /* r = moving bit from right to left */

    while (r != 0U) {
        t = s + r;

        if (t <= hx) {
            s    = t + r;
            hx  -= t;
            q   += r;
        }

        hx += hx;
        r >>= 1U;
    }

    if (hx != 0U) {
        (void) __raise_inexactf(x);
        q += (q & 1U);
    }

    ix = (q >> 1U) + 0x3f000000U;
    ix += m * 0x00800000U;
    SET_FLOAT_WORD(z, ix);
    return z;
}

#ifdef __LIBMCS_DOUBLE_IS_32BITS

double sqrt(double x)
{
    return (double) sqrtf((float) x);
}

#endif /* #ifdef __LIBMCS_DOUBLE_IS_32BITS */
