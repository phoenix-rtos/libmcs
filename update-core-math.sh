#!/bin/bash

set -xe

rm -rf libm/cr*

cp -r ../../extern/core-math/src/binary32/ libm/crmathf
cp -r ../../extern/core-math/src/binary64/ libm/crmathd
find libm/cr* -name "Makefile*" -delete \
           -o -name "*_mpfr.c" -delete \
           -o -name "check_*.c" -delete \
           -o -name function_under_test.h -delete
rm libm/crmathf/pow/exact.c \
    libm/crmathf/pow/worst.c \
    libm/crmathd/sinh/buildu.c \
    libm/crmathf/hypot/triples.c \
    libm/crmathd/hypot/triples.c \

mv libm/crmathd/log/dint.h libm/crmathd/log/log_dint.h
find libm/crmathd/log -exec sed -i 's/"dint\.h/"log_dint\.h/' {} \;
mv libm/crmathd/log10/dint.h libm/crmathd/log10/log10_dint.h
find libm/crmathd/log10 -exec sed -i 's/"dint\.h/"log10_dint\.h/' {} \;
mv libm/crmathd/log1p/dint.h libm/crmathd/log1p/log1p_dint.h
find libm/crmathd/log1p -exec sed -i 's/"dint\.h/"log1p_dint\.h/' {} \;
mv libm/crmathd/atan2/tint.h libm/crmathd/atan2/atan2_tint.h
find libm/crmathd/atan2 -exec sed -i 's/"tint\.h/"atan2_tint\.h/' {} \;
mv libm/crmathd/atan2pi/tint.h libm/crmathd/atan2pi/atan2pi_tint.h
find libm/crmathd/atan2pi -exec sed -i 's/"tint\.h/"atan2pi_tint\.h/' {} \;
mv libm/crmathd/pow/qint.h libm/crmathd/pow/pow_qint.h
find libm/crmathd/pow -exec sed -i 's/"qint\.h/"pow_qint\.h/' {} \;

find libm/crmathf -name "*.c" -exec mv {} libm/crmathf \;
find libm/crmathd -name "*.c" -exec mv {} libm/crmathd \;
find libm/crmathd -name "*dint.h" -exec mv {} libm/crmathd \;
find libm/crmathd -name "*tint.h" -exec mv {} libm/crmathd \;
find libm/crmathd -name "*qint.h" -exec mv {} libm/crmathd \;
mv libm/crmathd/pow/pow.h libm/crmathd/pow.h

for d in libm/cr*/*/; do
    rm -r "$d"
done

find libm/cr* -name "*.c" -exec sed -i 's/cr_\(.*\)/\1/' {} \;
