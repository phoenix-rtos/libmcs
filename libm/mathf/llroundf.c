/* SPDX-License-Identifier: SunMicrosystems */
/* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved. */
/* lrint adapted to be llrint for Newlib, 2009 by Craig Howland. */

#include <math.h>
#include "../common/tools.h"

long long int llroundf(float x)
{
    int32_t exponent_less_127;
    uint32_t w;
    long long int result;
    int32_t sign;

    GET_FLOAT_WORD(w, x);
    exponent_less_127 = (int32_t)((w & 0x7f800000U) >> 23) - 127;
    sign = (w & (uint32_t)0x80000000U) != 0U ? -1 : 1;
    w &= 0x7fffffU;
    w |= 0x800000U;

    if (exponent_less_127 < (int32_t)((8U * sizeof(long long int)) - 1U)) {
        if (exponent_less_127 < 0) {
            return (long long int)((exponent_less_127 < -1) ? 0 : sign);
        } else if (exponent_less_127 >= 23) {
            result = (long long int) (w << ((uint32_t)exponent_less_127 - 23U));
        } else {
            w += 0x400000U >> (uint32_t)exponent_less_127;
            result = w >> (uint32_t)(23 - exponent_less_127);
        }
    } else {
        (void) __raise_invalidf();
        if (sign == -1) {
            return LLONG_MIN;
        }
        else {
            return LLONG_MAX;
        }
    }

    return sign * result;
}

#ifdef __LIBMCS_DOUBLE_IS_32BITS

long long int llround(double x)
{
    return llroundf((float) x);
}

#endif /* #ifdef __LIBMCS_DOUBLE_IS_32BITS */
