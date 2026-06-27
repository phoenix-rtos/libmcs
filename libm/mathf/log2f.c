/* SPDX-License-Identifier: SunMicrosystems */
/* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved. */
/* Conversion to float by Ian Lance Taylor, Cygnus Support, ian@cygnus.com. */

#include <math.h>
#include "../common/tools.h"
#include "internal/log1pmff.h"

static const float
two25      =  3.3554432000e+07f, /* 0x4c000000 */
ivln2hi    =  1.4428710938e+00f, /* 0x3fb8b000 */
ivln2lo    = -1.7605285393e-04f; /* 0xb9389ad4 */

static const float zero = 0.0f;

float log2f(float x)
{
#ifdef __LIBMCS_FPU_DAZ
    x *= __volatile_onef;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    float f, hfsq, hi, lo, r, y;
    int32_t k;
    uint32_t hx, ix, i;

    GET_FLOAT_WORD(hx, x);
    ix = hx & 0x7fffffffU;

    k = 0;

    if (FLT_UWORD_IS_ZERO(ix)) {
        return __raise_div_by_zerof(-1.0f);     /* log(+-0)=-inf */
    }

    if (FLT_UWORD_IS_NAN(ix)) {    /* x = NaN */
        return x + x;
    }

    if ((int32_t)hx < 0) {
        return __raise_invalidf();              /* log(-#) = NaN */
    }

    if (FLT_UWORD_IS_INFINITE(ix)) {            /* x = +Inf */
        return x + x;
    }

    if (FLT_UWORD_IS_SUBNORMAL(ix)) {
        k -= 25;
        x *= two25; /* subnormal number, scale up x */
        GET_FLOAT_WORD(hx, x);
    }

    if (hx == 0x3f800000U) {                     /* log(1) = +0 */
        return zero;
    }

    k += (int32_t)(hx >> 23U) - 127;
    hx &= 0x007fffffU;
    i = (hx + 0x4afb0dU) & 0x800000U;
    SET_FLOAT_WORD(x, hx | (i ^ 0x3f800000U)); /* normalize x or x/2 */
    k += (int32_t)(i >> 23U);
    y = (float)k;
    f = x - 1.0f;
    hfsq = 0.5f * f * f;
    r = __log1pmff(f);
    hi = f - hfsq;
    GET_FLOAT_WORD(hx, hi);
    SET_FLOAT_WORD(hi, hx & 0xfffff000U);
    lo = (f - hi) - hfsq + r;
    return (lo + hi) * ivln2lo + lo * ivln2hi + hi * ivln2hi + y;
}

#ifdef __LIBMCS_DOUBLE_IS_32BITS

double log2(double x)
{
    return (double) log2f((float) x);
}

#endif /* #ifdef __LIBMCS_DOUBLE_IS_32BITS */
