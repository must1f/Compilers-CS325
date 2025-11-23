#!/bin/bash

# Improved test for "Did you mean?" functionality
# Tests each typo separately to avoid early termination

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m'

COMPILER="./mccomp"

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}Testing 'Did you mean?' Functionality${NC}"
echo -e "${BLUE}========================================${NC}"
echo ""

if [ ! -f "$COMPILER" ]; then
    echo -e "${RED}Error: Compiler '$COMPILER' not found${NC}"
    exit 1
fi

PASSED=0
FAILED=0

# Test 1: Typo 'countr' should suggest 'counter'
echo -e "${CYAN}Test 1: Variable typo 'countr' → 'counter'${NC}"
cat > /tmp/test_typo1.c << 'EOF'
int main() {
    int counter;
    int value;
    counter = 10;
    value = countr + 5;  // Typo: should be 'counter'
    return value;
}
EOF

output=$(timeout 5 $COMPILER /tmp/test_typo1.c 2>&1)
if echo "$output" | grep -qi "did you mean.*counter"; then
    echo -e "  ${GREEN}✓${NC} Correctly suggested 'counter'"
    PASSED=$((PASSED + 1))
else
    echo -e "  ${RED}✗${NC} Failed to suggest 'counter'"
    echo "$output"
    FAILED=$((FAILED + 1))
fi
echo ""

# Test 2: Typo 'temperatur' should suggest 'temperature'
echo -e "${CYAN}Test 2: Variable typo 'temperatur' → 'temperature'${NC}"
cat > /tmp/test_typo2.c << 'EOF'
int main() {
    float temperature;
    float result;
    temperature = 98.6;
    result = temperatur + 1.0;  // Typo: should be 'temperature'
    return 0;
}
EOF

output=$(timeout 5 $COMPILER /tmp/test_typo2.c 2>&1)
if echo "$output" | grep -qi "did you mean.*temperature"; then
    echo -e "  ${GREEN}✓${NC} Correctly suggested 'temperature'"
    PASSED=$((PASSED + 1))
else
    echo -e "  ${RED}✗${NC} Failed to suggest 'temperature'"
    echo "$output"
    FAILED=$((FAILED + 1))
fi
echo ""

# Test 3: Typo 'valu' should suggest 'value'
echo -e "${CYAN}Test 3: Variable typo 'valu' → 'value'${NC}"
cat > /tmp/test_typo3.c << 'EOF'
int main() {
    int value;
    int result;
    value = 42;
    result = valu * 2;  // Typo: should be 'value'
    return result;
}
EOF

output=$(timeout 5 $COMPILER /tmp/test_typo3.c 2>&1)
if echo "$output" | grep -qi "did you mean.*value"; then
    echo -e "  ${GREEN}✓${NC} Correctly suggested 'value'"
    PASSED=$((PASSED + 1))
else
    echo -e "  ${RED}✗${NC} Failed to suggest 'value'"
    echo "$output"
    FAILED=$((FAILED + 1))
fi
echo ""

# Test 4: Similar variable names - should pick closest match
echo -e "${CYAN}Test 4: Multiple similar variables 'indexx' → 'index'${NC}"
cat > /tmp/test_typo4.c << 'EOF'
int main() {
    int index;
    int indent;
    int number;
    index = 0;
    indent = 4;
    number = indexx;  // Typo: closest is 'index'
    return number;
}
EOF

output=$(timeout 5 $COMPILER /tmp/test_typo4.c 2>&1)
if echo "$output" | grep -qi "did you mean.*index"; then
    echo -e "  ${GREEN}✓${NC} Correctly suggested 'index' (closest match)"
    PASSED=$((PASSED + 1))
else
    echo -e "  ${RED}✗${NC} Failed to suggest 'index'"
    echo "$output"
    FAILED=$((FAILED + 1))
fi
echo ""

# Test 5: Check formatting elements
echo -e "${CYAN}Test 5: Error message formatting${NC}"
cat > /tmp/test_typo5.c << 'EOF'
int main() {
    int myVariable;
    myVariable = 10;
    return myVariabl;  // Typo
}
EOF

output=$(timeout 5 $COMPILER /tmp/test_typo5.c 2>&1)
checks_passed=0

if echo "$output" | grep -qi "hint:"; then
    echo -e "  ${GREEN}✓${NC} Has 'Hint:' label"
    checks_passed=$((checks_passed + 1))
else
    echo -e "  ${RED}✗${NC} Missing 'Hint:' label"
fi

if echo "$output" | grep -qi "did you mean"; then
    echo -e "  ${GREEN}✓${NC} Has 'Did you mean' text"
    checks_passed=$((checks_passed + 1))
else
    echo -e "  ${RED}✗${NC} Missing 'Did you mean' text"
fi

if echo "$output" | grep -qE "line [0-9]+"; then
    echo -e "  ${GREEN}✓${NC} Includes line number"
    checks_passed=$((checks_passed + 1))
else
    echo -e "  ${RED}✗${NC} Missing line number"
fi

if [ $checks_passed -eq 3 ]; then
    PASSED=$((PASSED + 1))
else
    FAILED=$((FAILED + 1))
fi
echo ""

# Summary
echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}Summary${NC}"
echo -e "${BLUE}========================================${NC}"
TOTAL=$((PASSED + FAILED))
echo -e "Total tests: $TOTAL"
echo -e "${GREEN}Passed: $PASSED${NC}"
echo -e "${RED}Failed: $FAILED${NC}"
echo ""

if [ $FAILED -eq 0 ]; then
    echo -e "${GREEN}✓ All tests passed! 'Did you mean?' is working perfectly!${NC}"
    exit 0
else
    percentage=$((PASSED * 100 / TOTAL))
    echo -e "${YELLOW}$percentage% tests passed${NC}"
    if [ $percentage -ge 80 ]; then
        echo -e "${GREEN}'Did you mean?' is working well!${NC}"
    else
        echo -e "${YELLOW}'Did you mean?' may need some adjustments${NC}"
    fi
    exit 1
fi

# Cleanup
rm -f /tmp/test_typo*.c
