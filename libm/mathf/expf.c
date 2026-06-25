/* SPDX-License-Identifier: SunMicrosystems */
/* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved. */
/* Conversion to float by Ian Lance Taylor, Cygnus Support, ian@cygnus.com. */

#include <math.h>
#include "../common/tools.h"

static const float
one       =   1.0f,
zero      =   0.0f,
halF[2]   = { 0.5f, -0.5f},
twom100   =   7.8886090522e-31f, /* 2**-100=0x0d800000 */
ln2HI[2]  = { 6.9314575195e-01f, /* 0x3f317200 */
             -6.9314575195e-01f, /* 0xbf317200 */
},
ln2LO[2]  = { 1.4286067653e-06f, /* 0x35bfbe8e */
             -1.4286067653e-06f, /* 0xb5bfbe8e */
},
invln2    =   1.4426950216e+00f, /* 0x3fb8aa3b */
P1        =   1.6666667163e-01f, /* 0x3e2aaaab */
P2        =  -2.7777778450e-03f, /* 0xbb360b61 */
P3        =   6.6137559770e-05f, /* 0x388ab355 */
P4        =  -1.6533901999e-06f, /* 0xb5ddea0e */
P5        =   4.1381369442e-08f; /* 0x3331bb4c */

float expf(float x)    /* default IEEE float exp */
{
#ifdef __LIBMCS_FPU_DAZ
    x *= __volatile_onef;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    float y, c, t;
    float hi = 0.0f;
    float lo = 0.0f;
    int32_t k = 0;
    int32_t sx;
    uint32_t hx;
    uint32_t ix;
    uint32_t xsb;

    GET_FLOAT_WORD(hx, x);
    xsb = (hx >> 31U) & 1U;    /* sign bit of x */
    ix = hx & 0x7fffffffU;    /* high word of |x| */

    /* filter out non-finite argument */
    if (FLT_UWORD_IS_NAN(ix)) {
        return x + x;    /* NaN */
    }

    if (FLT_UWORD_IS_INFINITE(ix)) {
        return (xsb == 0U) ? x : zero;
    }        /* exp(+-inf)={inf,0} */

    if ((int32_t)hx > (int32_t)FLT_UWORD_LOG_MAX) {
        return __raise_overflowf(one);    /* overflow */
    }

    if ((int32_t)hx < 0 && ix > FLT_UWORD_LOG_MIN) {
        return __raise_underflowf(zero);    /* underflow */
    }

    /* argument reduction */
    if (ix > 0x3eb17218U) {       /* if  |x| > 0.5 ln2 */
        if (ix < 0x3F851592U) {   /* and |x| < 1.5 ln2 */
            hi = x - ln2HI[xsb];
            lo = ln2LO[xsb];
            k = 1 - (int32_t)xsb - (int32_t)xsb;
        } else {
            k  = invln2 * x + halF[xsb];
            t  = k;
            hi = x - t * ln2HI[0];  /* t*ln2HI is exact here */
            lo = t * ln2LO[0];
        }

        x  = hi - lo;
    } else if (ix < 0x34000000U)  { /* when |x|<2**-23 */
        if (x == 0.0f) {            /* return 1 inexact except 0 */
            return one;
        } else {
            return __raise_inexactf(one + x);
        }
    } else {
        /* No action required */
    }

    /* x is now in primary range */
    t  = x * x;
    c  = x - t * (P1 + t * (P2 + t * (P3 + t * (P4 + t * P5))));

    if (k == 0) {
        return one - ((x * c) / (c - 2.0f) - x);
    } else {
        y = one - ((lo - (x * c) / (2.0f - c)) - hi);
    }

    if (k >= -125) {
        uint32_t hy;
        GET_FLOAT_WORD(hy, y);
        SET_FLOAT_WORD(y, hy + (((uint32_t)k) << 23U)); /* add k to y's exponent */
        return y;
    } else {
        uint32_t hy;
        GET_FLOAT_WORD(hy, y);
        SET_FLOAT_WORD(y, hy + (((uint32_t)k + 100U) << 23U)); /* add k to y's exponent */
        return y * twom100;
    }
}

#ifdef __LIBMCS_DOUBLE_IS_32BITS

double exp(double x)
{
    return (double) expf((float) x);
}

#endif /* #ifdef __LIBMCS_DOUBLE_IS_32BITS */
