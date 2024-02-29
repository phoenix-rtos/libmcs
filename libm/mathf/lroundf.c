/* SPDX-License-Identifier: SunMicrosystems */
/* Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved. */

#include <math.h>
#include "../common/tools.h"

long int lroundf(float x)
{
    int32_t exponent_less_127, sign;
    uint32_t w;
    long int result;

    GET_FLOAT_WORD(w, x);
    exponent_less_127 = (int32_t)((w & 0x7f800000U) >> 23U) - 127;
    sign = (w & (uint32_t)0x80000000U) != 0U ? -1 : 1;
    w &= 0x7fffffU;
    w |= 0x800000U;

    if (exponent_less_127 < (int32_t)((8U * sizeof(long int)) - 1U)) {
        if (exponent_less_127 < 0) {
            return exponent_less_127 < -1 ? 0 : sign;
        } else if (exponent_less_127 >= 23) {
            result = (long int) (w << (uint32_t)(exponent_less_127 - 23));
        } else {
            w += 0x400000U >> (uint32_t)exponent_less_127;
            result = w >> (uint32_t)(23 - exponent_less_127);
        }
    } else {
        (void) __raise_invalidf();
        if (sign == -1) {
            return __MIN_LONG;
        } else {
            return __MAX_LONG;
        }
    }

    return sign * result;
}

#ifdef __LIBMCS_DOUBLE_IS_32BITS

long int lround(double x)
{
    return lroundf((float) x);
}

#endif /* #ifdef __LIBMCS_DOUBLE_IS_32BITS */
