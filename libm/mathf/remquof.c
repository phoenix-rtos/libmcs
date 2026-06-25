/* SPDX-License-Identifier: SunMicrosystems */
/* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved. */

#include <assert.h>
#include <math.h>
#include "../common/tools.h"

static const float zero = 0.0f;

float remquof(float x, float y, int *quo)
{
#ifdef __LIBMCS_FPU_DAZ
    x *= __volatile_onef;
    y *= __volatile_onef;
#endif /* defined(__LIBMCS_FPU_DAZ) */

    int _quo = 0;
    uint32_t sx, sq, hx, hy;
    float y_half;

    assert(quo != (void*)0U);
    if (quo == (void*)0U) {
        quo = &_quo;
    }
    *quo = 0;

    GET_FLOAT_WORD(hx, x);
    GET_FLOAT_WORD(hy, y);
    sx = hx & 0x80000000U;
    sq = sx ^ (hy & 0x80000000U);
    hy &= 0x7fffffffU;
    hx &= 0x7fffffffU;

    /* purge off exception values */
    if (FLT_UWORD_IS_NAN(hx) || FLT_UWORD_IS_NAN(hy)) {                 /* x or y is NaN */
        return x + y;
    } else if (FLT_UWORD_IS_ZERO(hy) || FLT_UWORD_IS_INFINITE(hx)) {    /* y is 0 or x is inf */
        return __raise_invalidf();
    } else {
        /* No action required */
    }

    if (hy <= 0x7dffffffU) {
        x = fmodf(x, 8.0f * y);    /* now x < 8y */
    }

    if ((hx - hy) == 0U) {
        *quo = (sq != 0U) ? -1 : 1;
        return zero * x;
    }

    x  = fabsf(x);
    y  = fabsf(y);
    _quo = 0;

    if (x >= 4.0f * y) {
        x -= 4.0f * y;
        _quo += 4;
    }
    if (x >= 2.0f * y) {
        x -= 2.0f * y;
        _quo += 2;
    }

    if (hy < 0x01000000U) {
        if (x + x > y) {
            x -= y;
            _quo++;

            if (x + x >= y) {
                x -= y;
                _quo++;
            }
        }
    } else {
        y_half = 0.5f * y;

        if (x > y_half) {
            x -= y;
            _quo++;

            if (x >= y_half) {
                x -= y;
                _quo++;
            }
        }
    }

    _quo &= 0x7;

    *quo = (sq != 0U) ? -_quo : _quo;

    GET_FLOAT_WORD(hx, x);
    SET_FLOAT_WORD(x, hx ^ sx);
    return x;
}
