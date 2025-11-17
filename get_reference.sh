#!/bin/bash

# =============================================================================
# Reference IR Analysis Script - Get Exact Information Needed
# =============================================================================

OUTPUT="reference_ir_analysis.txt"

echo "LLVM Reference IR Analysis" > "$OUTPUT"
echo "Generated: $(date)" >> "$OUTPUT"
echo "=========================================================================" >> "$OUTPUT"
echo "" >> "$OUTPUT"

# =============================================================================
# PART 1: Simple Function (addition)
# =============================================================================

echo "╔═══════════════════════════════════════════════════════════════════════╗" >> "$OUTPUT"
echo "║  PART 1: SIMPLE FUNCTION - addition.c                                ║" >> "$OUTPUT"
echo "╚═══════════════════════════════════════════════════════════════════════╝" >> "$OUTPUT"
echo "" >> "$OUTPUT"

echo "SOURCE:" >> "$OUTPUT"
cat tests/addition/addition.c >> "$OUTPUT"
echo "" >> "$OUTPUT"
echo "─────────────────────────────────────────────────────────────────────────" >> "$OUTPUT"
echo "CLANG GENERATED IR:" >> "$OUTPUT"
cat tests/addition/reference.ll >> "$OUTPUT"
echo "" >> "$OUTPUT"
echo "" >> "$OUTPUT"

# =============================================================================
# PART 2: While Loop (factorial)
# =============================================================================

echo "╔═══════════════════════════════════════════════════════════════════════╗" >> "$OUTPUT"
echo "║  PART 2: WHILE LOOP - factorial.c                                    ║" >> "$OUTPUT"
echo "╚═══════════════════════════════════════════════════════════════════════╝" >> "$OUTPUT"
echo "" >> "$OUTPUT"

echo "SOURCE:" >> "$OUTPUT"
cat tests/factorial/factorial.c >> "$OUTPUT"
echo "" >> "$OUTPUT"
echo "─────────────────────────────────────────────────────────────────────────" >> "$OUTPUT"
echo "CLANG GENERATED IR:" >> "$OUTPUT"
cat tests/factorial/reference.ll >> "$OUTPUT"
echo "" >> "$OUTPUT"
echo "" >> "$OUTPUT"

# =============================================================================
# PART 3: Unary Operators (unary)
# =============================================================================

echo "╔═══════════════════════════════════════════════════════════════════════╗" >> "$OUTPUT"
echo "║  PART 3: UNARY OPERATORS - unary.c                                   ║" >> "$OUTPUT"
echo "╚═══════════════════════════════════════════════════════════════════════╝" >> "$OUTPUT"
echo "" >> "$OUTPUT"

echo "SOURCE:" >> "$OUTPUT"
cat tests/unary/unary.c >> "$OUTPUT"
echo "" >> "$OUTPUT"
echo "─────────────────────────────────────────────────────────────────────────" >> "$OUTPUT"
echo "CLANG GENERATED IR:" >> "$OUTPUT"
head -150 tests/unary/reference.ll >> "$OUTPUT"
echo "" >> "$OUTPUT"
echo "[... truncated for length ...]" >> "$OUTPUT"
echo "" >> "$OUTPUT"
echo "" >> "$OUTPUT"

# =============================================================================
# PART 4: Void Function (void)
# =============================================================================

echo "╔═══════════════════════════════════════════════════════════════════════╗" >> "$OUTPUT"
echo "║  PART 4: VOID FUNCTION - void.c                                      ║" >> "$OUTPUT"
echo "╚═══════════════════════════════════════════════════════════════════════╝" >> "$OUTPUT"
echo "" >> "$OUTPUT"

echo "SOURCE:" >> "$OUTPUT"
cat tests/void/void.c >> "$OUTPUT"
echo "" >> "$OUTPUT"
echo "─────────────────────────────────────────────────────────────────────────" >> "$OUTPUT"
echo "CLANG GENERATED IR:" >> "$OUTPUT"
cat tests/void/reference.ll >> "$OUTPUT"
echo "" >> "$OUTPUT"
echo "" >> "$OUTPUT"

# =============================================================================
# PART 5: Recursion (recurse)
# =============================================================================

echo "╔═══════════════════════════════════════════════════════════════════════╗" >> "$OUTPUT"
echo "║  PART 5: RECURSION - recurse.c                                       ║" >> "$OUTPUT"
echo "╚═══════════════════════════════════════════════════════════════════════╝" >> "$OUTPUT"
echo "" >> "$OUTPUT"

echo "SOURCE:" >> "$OUTPUT"
cat tests/recurse/recurse.c >> "$OUTPUT"
echo "" >> "$OUTPUT"
echo "─────────────────────────────────────────────────────────────────────────" >> "$OUTPUT"
echo "CLANG GENERATED IR:" >> "$OUTPUT"
head -120 tests/recurse/reference.ll >> "$OUTPUT"
echo "" >> "$OUTPUT"
echo "[... truncated for length ...]" >> "$OUTPUT"
echo "" >> "$OUTPUT"
echo "" >> "$OUTPUT"

# =============================================================================
# PART 6: Fix Failed Tests
# =============================================================================

echo "╔═══════════════════════════════════════════════════════════════════════╗" >> "$OUTPUT"
echo "║  PART 6: FIXING FAILED TESTS                                         ║" >> "$OUTPUT"
echo "╚═══════════════════════════════════════════════════════════════════════╝" >> "$OUTPUT"
echo "" >> "$OUTPUT"

echo "Attempting to generate IR for failed tests..." >> "$OUTPUT"
echo "" >> "$OUTPUT"

# Try palindrome
echo "─── PALINDROME ───" >> "$OUTPUT"
clang -S -emit-llvm -O0 tests/palindrome/palindrome.c -o tests/palindrome/reference.ll 2>&1 >> "$OUTPUT"
if [ $? -eq 0 ]; then
    echo "✓ Success!" >> "$OUTPUT"
    head -100 tests/palindrome/reference.ll >> "$OUTPUT"
else
    echo "✗ Failed - Error above" >> "$OUTPUT"
fi
echo "" >> "$OUTPUT"

# Try pi
echo "─── PI ───" >> "$OUTPUT"
clang -S -emit-llvm -O0 tests/pi/pi.c -o tests/pi/reference.ll 2>&1 >> "$OUTPUT"
if [ $? -eq 0 ]; then
    echo "✓ Success!" >> "$OUTPUT"
    head -100 tests/pi/reference.ll >> "$OUTPUT"
else
    echo "✗ Failed - Error above" >> "$OUTPUT"
fi
echo "" >> "$OUTPUT"

# Try while
echo "─── WHILE ───" >> "$OUTPUT"
clang -S -emit-llvm -O0 tests/while/while.c -o tests/while/reference.ll 2>&1 >> "$OUTPUT"
if [ $? -eq 0 ]; then
    echo "✓ Success!" >> "$OUTPUT"
    head -100 tests/while/reference.ll >> "$OUTPUT"
else
    echo "✗ Failed - Error above" >> "$OUTPUT"
fi
echo "" >> "$OUTPUT"

# =============================================================================
# PART 7: Key Patterns to Extract
# =============================================================================

echo "╔═══════════════════════════════════════════════════════════════════════╗" >> "$OUTPUT"
echo "║  PART 7: KEY IR PATTERNS                                             ║" >> "$OUTPUT"
echo "╚═══════════════════════════════════════════════════════════════════════╝" >> "$OUTPUT"
echo "" >> "$OUTPUT"

echo "PATTERN 1: Function Declaration" >> "$OUTPUT"
echo "─────────────────────────────────────────────────────────────────────────" >> "$OUTPUT"
grep "^define" tests/addition/reference.ll | head -3 >> "$OUTPUT"
echo "" >> "$OUTPUT"

echo "PATTERN 2: Alloca Instructions (local variables)" >> "$OUTPUT"
echo "─────────────────────────────────────────────────────────────────────────" >> "$OUTPUT"
grep "alloca" tests/addition/reference.ll | head -5 >> "$OUTPUT"
echo "" >> "$OUTPUT"

echo "PATTERN 3: Store Instructions (assignment)" >> "$OUTPUT"
echo "─────────────────────────────────────────────────────────────────────────" >> "$OUTPUT"
grep "store" tests/addition/reference.ll | head -5 >> "$OUTPUT"
echo "" >> "$OUTPUT"

echo "PATTERN 4: Load Instructions (variable read)" >> "$OUTPUT"
echo "─────────────────────────────────────────────────────────────────────────" >> "$OUTPUT"
grep "load" tests/addition/reference.ll | head -5 >> "$OUTPUT"
echo "" >> "$OUTPUT"

echo "PATTERN 5: Basic Blocks (control flow)" >> "$OUTPUT"
echo "─────────────────────────────────────────────────────────────────────────" >> "$OUTPUT"
grep -E "^[0-9]+:" tests/factorial/reference.ll | head -10 >> "$OUTPUT"
echo "" >> "$OUTPUT"

echo "PATTERN 6: Branch Instructions" >> "$OUTPUT"
echo "─────────────────────────────────────────────────────────────────────────" >> "$OUTPUT"
grep "br " tests/factorial/reference.ll | head -5 >> "$OUTPUT"
echo "" >> "$OUTPUT"

echo "PATTERN 7: Comparison Instructions" >> "$OUTPUT"
echo "─────────────────────────────────────────────────────────────────────────" >> "$OUTPUT"
grep "icmp\|fcmp" tests/factorial/reference.ll | head -5 >> "$OUTPUT"
echo "" >> "$OUTPUT"

# =============================================================================
# COMPLETION
# =============================================================================

echo "" >> "$OUTPUT"
echo "═════════════════════════════════════════════════════════════════════════" >> "$OUTPUT"
echo "Analysis Complete" >> "$OUTPUT"
echo "File: $OUTPUT" >> "$OUTPUT"
echo "Size: $(wc -l < "$OUTPUT") lines" >> "$OUTPUT"
echo "═════════════════════════════════════════════════════════════════════════" >> "$OUTPUT"

# Print summary to console
echo "✓ Analysis complete!"
echo "✓ Output: $OUTPUT"
echo "✓ Size: $(du -h "$OUTPUT" | cut -f1)"
echo ""
echo "Upload this file to Claude for full codegen implementation."