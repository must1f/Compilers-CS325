#!/bin/bash
set -e

# Setup environment
export LLVM_INSTALL_PATH=/modules/cs325/llvm-21.1.0
export PATH=$LLVM_INSTALL_PATH/bin:$PATH
export LD_LIBRARY_PATH=$LLVM_INSTALL_PATH/lib:$LD_LIBRARY_PATH
module load GCC/13.3.0

echo "=========================================="
echo "Testing MiniC Compiler - Section 2.1.3"
echo "=========================================="

# Build compiler
echo ""
echo "Building compiler..."
make clean
make mccomp

TEST_FILE="tests/addition/addition.c"
echo ""
echo "Testing with: $TEST_FILE"

# Step 1: Compile to LLVM IR
echo ""
echo "Step 1: Generating LLVM IR..."
./mccomp "$TEST_FILE"

if [ ! -f "output.ll" ]; then
    echo "✗ FAILED: output.ll not generated"
    exit 1
fi
echo "✓ output.ll generated"

# Step 2: Validate with lli (LLVM Interpreter)
echo ""
echo "Step 2: Validating IR with lli (LLVM Interpreter)..."
if lli output.ll 2>&1 | grep -q "error"; then
    echo "✗ FAILED: IR validation failed"
    lli output.ll
    exit 1
else
    echo "✓ IR is valid (lli accepts it)"
fi

# Step 3: Compile to assembly with llc
echo ""
echo "Step 3: Compiling IR to assembly with llc..."
if llc output.ll 2>&1 | grep -q "error"; then
    echo "✗ FAILED: Could not compile to assembly"
    llc output.ll
    exit 1
else
    echo "✓ Compiled to output.s successfully"
    ls -lh output.s
fi

# Step 4: Build executable with clang++
echo ""
echo "Step 4: Building executable..."
DRIVER_FILE="tests/addition/driver.cpp"
if [ -f "$DRIVER_FILE" ]; then
    clang++ "$DRIVER_FILE" output.ll -o test_exe
    echo "✓ Executable built: test_exe"
    
    # Step 5: Run and check result
    echo ""
    echo "Step 5: Running executable..."
    ./test_exe
    
    if ./test_exe 2>&1 | grep -q "PASSED"; then
        echo "✓ TEST PASSED!"
    else
        echo "✗ TEST FAILED!"
    fi
else
    echo "⚠ Skipping executable test (no driver)"
fi

# Step 6: Compare with clang reference (optional)
echo ""
echo "Step 6: Generating reference IR with clang..."
clang -S -emit-llvm "$TEST_FILE" -o reference.ll 2>/dev/null || echo "⚠ clang comparison skipped"

if [ -f "reference.ll" ]; then
    echo ""
    echo "Reference IR generated. Compare with:"
    echo "  diff output.ll reference.ll"
    echo "  or"
    echo "  vimdiff output.ll reference.ll"
fi

echo ""
echo "=========================================="
echo "All validation steps completed!"
echo "=========================================="