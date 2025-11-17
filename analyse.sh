#!/bin/bash

# =============================================================================
# Compiler Analysis Script - Gather All Required Information
# =============================================================================

OUTPUT_FILE="compiler_analysis.txt"

echo "Gathering compiler information..." > "$OUTPUT_FILE"
echo "Generated on: $(date)" >> "$OUTPUT_FILE"
echo "=========================================================================" >> "$OUTPUT_FILE"
echo "" >> "$OUTPUT_FILE"

# =============================================================================
# PART 1: Test File Contents
# =============================================================================

echo "╔═══════════════════════════════════════════════════════════════════════╗" >> "$OUTPUT_FILE"
echo "║                    PART 1: TEST SOURCE FILES                          ║" >> "$OUTPUT_FILE"
echo "╚═══════════════════════════════════════════════════════════════════════╝" >> "$OUTPUT_FILE"
echo "" >> "$OUTPUT_FILE"

# Null test (likely simplest)
echo "═════════════════════════════════════════════════════════════════════════" >> "$OUTPUT_FILE"
echo "TEST: null.c (Simplest Test)" >> "$OUTPUT_FILE"
echo "═════════════════════════════════════════════════════════════════════════" >> "$OUTPUT_FILE"
if [ -f "tests/null/null.c" ]; then
    cat tests/null/null.c >> "$OUTPUT_FILE"
else
    echo "[FILE NOT FOUND]" >> "$OUTPUT_FILE"
fi
echo "" >> "$OUTPUT_FILE"
echo "" >> "$OUTPUT_FILE"

# Addition test
echo "═════════════════════════════════════════════════════════════════════════" >> "$OUTPUT_FILE"
echo "TEST: addition.c" >> "$OUTPUT_FILE"
echo "═════════════════════════════════════════════════════════════════════════" >> "$OUTPUT_FILE"
if [ -f "tests/addition/addition.c" ]; then
    cat tests/addition/addition.c >> "$OUTPUT_FILE"
else
    echo "[FILE NOT FOUND]" >> "$OUTPUT_FILE"
fi
echo "" >> "$OUTPUT_FILE"
echo "" >> "$OUTPUT_FILE"

# Factorial test
echo "═════════════════════════════════════════════════════════════════════════" >> "$OUTPUT_FILE"
echo "TEST: factorial.c" >> "$OUTPUT_FILE"
echo "═════════════════════════════════════════════════════════════════════════" >> "$OUTPUT_FILE"
if [ -f "tests/factorial/factorial.c" ]; then
    cat tests/factorial/factorial.c >> "$OUTPUT_FILE"
else
    echo "[FILE NOT FOUND]" >> "$OUTPUT_FILE"
fi
echo "" >> "$OUTPUT_FILE"
echo "" >> "$OUTPUT_FILE"

# While test
echo "═════════════════════════════════════════════════════════════════════════" >> "$OUTPUT_FILE"
echo "TEST: while.c" >> "$OUTPUT_FILE"
echo "═════════════════════════════════════════════════════════════════════════" >> "$OUTPUT_FILE"
if [ -f "tests/while/while.c" ]; then
    cat tests/while/while.c >> "$OUTPUT_FILE"
else
    echo "[FILE NOT FOUND]" >> "$OUTPUT_FILE"
fi
echo "" >> "$OUTPUT_FILE"
echo "" >> "$OUTPUT_FILE"

# Unary test
echo "═════════════════════════════════════════════════════════════════════════" >> "$OUTPUT_FILE"
echo "TEST: unary.c" >> "$OUTPUT_FILE"
echo "═════════════════════════════════════════════════════════════════════════" >> "$OUTPUT_FILE"
if [ -f "tests/unary/unary.c" ]; then
    cat tests/unary/unary.c >> "$OUTPUT_FILE"
else
    echo "[FILE NOT FOUND]" >> "$OUTPUT_FILE"
fi
echo "" >> "$OUTPUT_FILE"
echo "" >> "$OUTPUT_FILE"

# Pi test (float heavy)
echo "═════════════════════════════════════════════════════════════════════════" >> "$OUTPUT_FILE"
echo "TEST: pi.c (Float operations)" >> "$OUTPUT_FILE"
echo "═════════════════════════════════════════════════════════════════════════" >> "$OUTPUT_FILE"
if [ -f "tests/pi/pi.c" ]; then
    cat tests/pi/pi.c >> "$OUTPUT_FILE"
else
    echo "[FILE NOT FOUND]" >> "$OUTPUT_FILE"
fi
echo "" >> "$OUTPUT_FILE"
echo "" >> "$OUTPUT_FILE"

# =============================================================================
# PART 2: Reference LLVM IR Files
# =============================================================================

echo "" >> "$OUTPUT_FILE"
echo "╔═══════════════════════════════════════════════════════════════════════╗" >> "$OUTPUT_FILE"
echo "║                    PART 2: REFERENCE LLVM IR                          ║" >> "$OUTPUT_FILE"
echo "╚═══════════════════════════════════════════════════════════════════════╝" >> "$OUTPUT_FILE"
echo "" >> "$OUTPUT_FILE"

# Addition IR
echo "═════════════════════════════════════════════════════════════════════════" >> "$OUTPUT_FILE"
echo "REFERENCE IR: addition/output.ll" >> "$OUTPUT_FILE"
echo "═════════════════════════════════════════════════════════════════════════" >> "$OUTPUT_FILE"
if [ -f "tests/addition/output.ll" ]; then
    cat tests/addition/output.ll >> "$OUTPUT_FILE"
else
    echo "[FILE NOT FOUND]" >> "$OUTPUT_FILE"
fi
echo "" >> "$OUTPUT_FILE"
echo "" >> "$OUTPUT_FILE"

# Factorial IR
echo "═════════════════════════════════════════════════════════════════════════" >> "$OUTPUT_FILE"
echo "REFERENCE IR: factorial/output.ll" >> "$OUTPUT_FILE"
echo "═════════════════════════════════════════════════════════════════════════" >> "$OUTPUT_FILE"
if [ -f "tests/factorial/output.ll" ]; then
    cat tests/factorial/output.ll >> "$OUTPUT_FILE"
else
    echo "[FILE NOT FOUND]" >> "$OUTPUT_FILE"
fi
echo "" >> "$OUTPUT_FILE"
echo "" >> "$OUTPUT_FILE"

# While IR
echo "═════════════════════════════════════════════════════════════════════════" >> "$OUTPUT_FILE"
echo "REFERENCE IR: while/output.ll" >> "$OUTPUT_FILE"
echo "═════════════════════════════════════════════════════════════════════════" >> "$OUTPUT_FILE"
if [ -f "tests/while/output.ll" ]; then
    cat tests/while/output.ll >> "$OUTPUT_FILE"
else
    echo "[FILE NOT FOUND]" >> "$OUTPUT_FILE"
fi
echo "" >> "$OUTPUT_FILE"
echo "" >> "$OUTPUT_FILE"

# Unary IR
echo "═════════════════════════════════════════════════════════════════════════" >> "$OUTPUT_FILE"
echo "REFERENCE IR: unary/output.ll" >> "$OUTPUT_FILE"
echo "═════════════════════════════════════════════════════════════════════════" >> "$OUTPUT_FILE"
if [ -f "tests/unary/output.ll" ]; then
    cat tests/unary/output.ll >> "$OUTPUT_FILE"
else
    echo "[FILE NOT FOUND]" >> "$OUTPUT_FILE"
fi
echo "" >> "$OUTPUT_FILE"
echo "" >> "$OUTPUT_FILE"

# Pi IR
echo "═════════════════════════════════════════════════════════════════════════" >> "$OUTPUT_FILE"
echo "REFERENCE IR: pi/output.ll" >> "$OUTPUT_FILE"
echo "═════════════════════════════════════════════════════════════════════════" >> "$OUTPUT_FILE"
if [ -f "tests/pi/output.ll" ]; then
    cat tests/pi/output.ll >> "$OUTPUT_FILE"
else
    echo "[FILE NOT FOUND]" >> "$OUTPUT_FILE"
fi
echo "" >> "$OUTPUT_FILE"
echo "" >> "$OUTPUT_FILE"

# Cosine IR
echo "═════════════════════════════════════════════════════════════════════════" >> "$OUTPUT_FILE"
echo "REFERENCE IR: cosine/output.ll" >> "$OUTPUT_FILE"
echo "═════════════════════════════════════════════════════════════════════════" >> "$OUTPUT_FILE"
if [ -f "tests/cosine/output.ll" ]; then
    cat tests/cosine/output.ll >> "$OUTPUT_FILE"
else
    echo "[FILE NOT FOUND]" >> "$OUTPUT_FILE"
fi
echo "" >> "$OUTPUT_FILE"
echo "" >> "$OUTPUT_FILE"

# Void IR
echo "═════════════════════════════════════════════════════════════════════════" >> "$OUTPUT_FILE"
echo "REFERENCE IR: void/output.ll" >> "$OUTPUT_FILE"
echo "═════════════════════════════════════════════════════════════════════════" >> "$OUTPUT_FILE"
if [ -f "tests/void/output.ll" ]; then
    cat tests/void/output.ll >> "$OUTPUT_FILE"
else
    echo "[FILE NOT FOUND]" >> "$OUTPUT_FILE"
fi
echo "" >> "$OUTPUT_FILE"
echo "" >> "$OUTPUT_FILE"

# Recurse IR
echo "═════════════════════════════════════════════════════════════════════════" >> "$OUTPUT_FILE"
echo "REFERENCE IR: recurse/output.ll" >> "$OUTPUT_FILE"
echo "═════════════════════════════════════════════════════════════════════════" >> "$OUTPUT_FILE"
if [ -f "tests/recurse/output.ll" ]; then
    cat tests/recurse/output.ll >> "$OUTPUT_FILE"
else
    echo "[FILE NOT FOUND]" >> "$OUTPUT_FILE"
fi
echo "" >> "$OUTPUT_FILE"
echo "" >> "$OUTPUT_FILE"

# Rfact IR
echo "═════════════════════════════════════════════════════════════════════════" >> "$OUTPUT_FILE"
echo "REFERENCE IR: rfact/output.ll" >> "$OUTPUT_FILE"
echo "═════════════════════════════════════════════════════════════════════════" >> "$OUTPUT_FILE"
if [ -f "tests/rfact/output.ll" ]; then
    cat tests/rfact/output.ll >> "$OUTPUT_FILE"
else
    echo "[FILE NOT FOUND]" >> "$OUTPUT_FILE"
fi
echo "" >> "$OUTPUT_FILE"
echo "" >> "$OUTPUT_FILE"

# Palindrome IR
echo "═════════════════════════════════════════════════════════════════════════" >> "$OUTPUT_FILE"
echo "REFERENCE IR: palindrome/output.ll" >> "$OUTPUT_FILE"
echo "═════════════════════════════════════════════════════════════════════════" >> "$OUTPUT_FILE"
if [ -f "tests/palindrome/output.ll" ]; then
    cat tests/palindrome/output.ll >> "$OUTPUT_FILE"
else
    echo "[FILE NOT FOUND]" >> "$OUTPUT_FILE"
fi
echo "" >> "$OUTPUT_FILE"
echo "" >> "$OUTPUT_FILE"

# Leap IR
echo "═════════════════════════════════════════════════════════════════════════" >> "$OUTPUT_FILE"
echo "REFERENCE IR: leap/output.ll" >> "$OUTPUT_FILE"
echo "═════════════════════════════════════════════════════════════════════════" >> "$OUTPUT_FILE"
if [ -f "tests/leap/output.ll" ]; then
    cat tests/leap/output.ll >> "$OUTPUT_FILE"
else
    echo "[FILE NOT FOUND]" >> "$OUTPUT_FILE"
fi
echo "" >> "$OUTPUT_FILE"
echo "" >> "$OUTPUT_FILE"

# =============================================================================
# PART 3: Driver Files
# =============================================================================

echo "" >> "$OUTPUT_FILE"
echo "╔═══════════════════════════════════════════════════════════════════════╗" >> "$OUTPUT_FILE"
echo "║                    PART 3: DRIVER FILES                               ║" >> "$OUTPUT_FILE"
echo "╚═══════════════════════════════════════════════════════════════════════╝" >> "$OUTPUT_FILE"
echo "" >> "$OUTPUT_FILE"

# Addition driver
echo "═════════════════════════════════════════════════════════════════════════" >> "$OUTPUT_FILE"
echo "DRIVER: addition/driver.cpp" >> "$OUTPUT_FILE"
echo "═════════════════════════════════════════════════════════════════════════" >> "$OUTPUT_FILE"
if [ -f "tests/addition/driver.cpp" ]; then
    cat tests/addition/driver.cpp >> "$OUTPUT_FILE"
else
    echo "[FILE NOT FOUND]" >> "$OUTPUT_FILE"
fi
echo "" >> "$OUTPUT_FILE"
echo "" >> "$OUTPUT_FILE"

# Factorial driver
echo "═════════════════════════════════════════════════════════════════════════" >> "$OUTPUT_FILE"
echo "DRIVER: factorial/driver.cpp" >> "$OUTPUT_FILE"
echo "═════════════════════════════════════════════════════════════════════════" >> "$OUTPUT_FILE"
if [ -f "tests/factorial/driver.cpp" ]; then
    cat tests/factorial/driver.cpp >> "$OUTPUT_FILE"
else
    echo "[FILE NOT FOUND]" >> "$OUTPUT_FILE"
fi
echo "" >> "$OUTPUT_FILE"
echo "" >> "$OUTPUT_FILE"

# While driver
echo "═════════════════════════════════════════════════════════════════════════" >> "$OUTPUT_FILE"
echo "DRIVER: while/driver.cpp" >> "$OUTPUT_FILE"
echo "═════════════════════════════════════════════════════════════════════════" >> "$OUTPUT_FILE"
if [ -f "tests/while/driver.cpp" ]; then
    cat tests/while/driver.cpp >> "$OUTPUT_FILE"
else
    echo "[FILE NOT FOUND]" >> "$OUTPUT_FILE"
fi
echo "" >> "$OUTPUT_FILE"
echo "" >> "$OUTPUT_FILE"

# =============================================================================
# PART 4: Summary Statistics
# =============================================================================

echo "" >> "$OUTPUT_FILE"
echo "╔═══════════════════════════════════════════════════════════════════════╗" >> "$OUTPUT_FILE"
echo "║                    PART 4: SUMMARY STATISTICS                         ║" >> "$OUTPUT_FILE"
echo "╚═══════════════════════════════════════════════════════════════════════╝" >> "$OUTPUT_FILE"
echo "" >> "$OUTPUT_FILE"

echo "Test directories found:" >> "$OUTPUT_FILE"
ls -d tests/*/ 2>/dev/null | wc -l >> "$OUTPUT_FILE"
echo "" >> "$OUTPUT_FILE"

echo "Tests with reference output.ll:" >> "$OUTPUT_FILE"
find tests -name "output.ll" | wc -l >> "$OUTPUT_FILE"
echo "" >> "$OUTPUT_FILE"

echo "List of all test directories:" >> "$OUTPUT_FILE"
ls -1 tests/ | grep -v "tests.sh" >> "$OUTPUT_FILE"
echo "" >> "$OUTPUT_FILE"

echo "Tests WITH reference IR:" >> "$OUTPUT_FILE"
for dir in tests/*/; do
    if [ -f "${dir}output.ll" ]; then
        basename "$dir"
    fi
done >> "$OUTPUT_FILE"
echo "" >> "$OUTPUT_FILE"

echo "Tests WITHOUT reference IR (need to generate):" >> "$OUTPUT_FILE"
for dir in tests/*/; do
    if [ ! -f "${dir}output.ll" ] && [ -f "${dir}"*.c ]; then
        basename "$dir"
    fi
done >> "$OUTPUT_FILE"
echo "" >> "$OUTPUT_FILE"

# =============================================================================
# PART 5: Current Compiler State
# =============================================================================

echo "" >> "$OUTPUT_FILE"
echo "╔═══════════════════════════════════════════════════════════════════════╗" >> "$OUTPUT_FILE"
echo "║                    PART 5: CURRENT COMPILER STATE                     ║" >> "$OUTPUT_FILE"
echo "╚═══════════════════════════════════════════════════════════════════════╝" >> "$OUTPUT_FILE"
echo "" >> "$OUTPUT_FILE"

echo "Checking for critical infrastructure in mccomp.cpp:" >> "$OUTPUT_FILE"
echo "" >> "$OUTPUT_FILE"

echo "Global variables declared:" >> "$OUTPUT_FILE"
grep -n "^static.*TheContext\|^static.*Builder\|^static.*TheModule" mccomp.cpp >> "$OUTPUT_FILE"
echo "" >> "$OUTPUT_FILE"

echo "Symbol tables (should be MISSING):" >> "$OUTPUT_FILE"
grep -n "NamedValues\|GlobalValues\|VariableTypes" mccomp.cpp | head -5 >> "$OUTPUT_FILE"
echo "" >> "$OUTPUT_FILE"

echo "Type helper function (should be MISSING):" >> "$OUTPUT_FILE"
grep -n "getTypeFromString" mccomp.cpp | head -3 >> "$OUTPUT_FILE"
echo "" >> "$OUTPUT_FILE"

echo "Codegen methods implemented:" >> "$OUTPUT_FILE"
grep -n "::codegen()" mccomp.cpp | grep -v "virtual Value \*codegen()" | head -10 >> "$OUTPUT_FILE"
echo "" >> "$OUTPUT_FILE"

# =============================================================================
# COMPLETION
# =============================================================================

echo "" >> "$OUTPUT_FILE"
echo "═════════════════════════════════════════════════════════════════════════" >> "$OUTPUT_FILE"
echo "Analysis complete. Total lines: $(wc -l < "$OUTPUT_FILE")" >> "$OUTPUT_FILE"
echo "═════════════════════════════════════════════════════════════════════════" >> "$OUTPUT_FILE"

# Print to console
echo "✓ Analysis complete!"
echo "✓ Output saved to: $OUTPUT_FILE"
echo "✓ Total lines: $(wc -l < "$OUTPUT_FILE")"
echo ""
echo "To view the file:"
echo "  less $OUTPUT_FILE"
echo ""
echo "To upload to Claude:"
echo "  1. Copy the file to your local machine"
echo "  2. Upload via the attachment button"
echo ""
echo "File size: $(du -h "$OUTPUT_FILE" | cut -f1)"