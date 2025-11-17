#!/bin/bash
# Clean everything
make clean
rm -f output.ll
find tests -name "output.ll" -delete
find tests -name "pi" -type f -delete

# Rebuild
make

# Test just pi
cd tests/pi
rm -f output.ll pi
../../mccomp pi.c
echo "=== Checking output.ll was created ==="
ls -lh output.ll
echo "=== First 20 lines of IR ==="
head -20 output.ll
echo "=== Building executable ==="
clang++ driver.cpp output.ll -o pi
echo "=== Running ==="
./pi