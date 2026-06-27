/* SPDX-License-Identifier: RedHat */
/* Copyright (C) 2002 by  Red Hat, Incorporated. All rights reserved. */

#include <math.h>
#include "../common/tools.h"

float fdimf(float x, float y)
{
#ifdef __LIBMCS_FPU_DAZ
    x *= __volatile_onef;
    y *= __volatile_onef;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    uint32_t hx, hy;
    GET_FLOAT_WORD(hx, x);
    GET_FLOAT_WORD(hy, y);

    if (FLT_UWORD_IS_NAN(hx & 0x7fffffffU) || FLT_UWORD_IS_NAN(hy & 0x7fffffffU)) {
        return x * y;
    }

    return x > y ? x - y : 0.0f;
}

#ifdef __LIBMCS_DOUBLE_IS_32BITS

double fdim(double x, double y)
{
    return (double) fdimf((float) x, (float) y);
}

#endif /* #ifdef __LIBMCS_DOUBLE_IS_32BITS */
