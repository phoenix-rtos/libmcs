/* SPDX-License-Identifier: GTDGmbH */
/* Copyright 2020-2021 by GTD GmbH. */

#ifndef LIBMCS_FENV_H
#define LIBMCS_FENV_H

#ifdef __cplusplus
extern "C"{
#endif

#define FE_TONEAREST  0
#define FE_INVALID    1
#define FE_DOWNWARD   1024
#define FE_UPWARD     2048
#define FE_TOWARDZERO 3072

/* Type representing exception flags.  */
typedef unsigned short int fexcept_t;

typedef struct
{
    unsigned short int __control_word;
    unsigned short int __glibc_reserved1;
    unsigned short int __status_word;
    unsigned short int __glibc_reserved2;
    unsigned short int __tags;
    unsigned short int __glibc_reserved3;
    unsigned int __eip;
    unsigned short int __cs_selector;
    unsigned int __opcode:11;
    unsigned int __glibc_reserved4:5;
    unsigned int __data_offset;
    unsigned short int __data_selector;
    unsigned short int __glibc_reserved5;
#ifdef __x86_64__
    unsigned int __mxcsr;
#endif
}
fenv_t;

/* Floating-point Exceptions */
extern int feclearexcept(int);
extern int feraiseexcept(int);
extern int fegetexceptflag(fexcept_t *, int);
extern int fesetexceptflag(const fexcept_t *, int);

/* Rounding Direction */
extern int fegetround();
extern int fesetround(int);

/* Entire Environment */
extern int fegetenv(fenv_t *);
extern int fesetenv(const fenv_t *);
extern int feholdexcept(fenv_t *);
extern int feupdateenv(const fenv_t *);

/* Other */
extern int fetestexcept(int);

#ifdef __cplusplus
}
#endif

#endif /* !LIBMCS_FENV_H */
