#!/bin/bash

# Test script to verify "Did you mean?" functionality
# This checks if the compiler provides helpful suggestions for typos

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m'

COMPILER="./mccomp"
TEST_FILE="test_did_you_mean.c"

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}Testing 'Did you mean?' Functionality${NC}"
echo -e "${BLUE}========================================${NC}"
echo ""

if [ ! -f "$COMPILER" ]; then
    echo -e "${RED}Error: Compiler '$COMPILER' not found${NC}"
    exit 1
fi

if [ ! -f "$TEST_FILE" ]; then
    echo -e "${RED}Error: Test file '$TEST_FILE' not found${NC}"
    exit 1
fi

echo -e "${CYAN}Running compiler on test file with typos...${NC}"
echo ""

# Run compiler and capture output
output=$(timeout 5 "$COMPILER" "$TEST_FILE" 2>&1)
exit_code=$?

echo "$output"
echo ""

# Check for "Did you mean?" suggestions
echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}Test Results${NC}"
echo -e "${BLUE}========================================${NC}"
echo ""

PASSED=0
FAILED=0

# Test 1: Check if error is detected for 'countr'
echo -e "${YELLOW}Test 1:${NC} Checking for undefined variable 'countr'"
if echo "$output" | grep -qi "countr"; then
    echo -e "  ${GREEN}✓${NC} Undefined variable 'countr' detected"
    PASSED=$((PASSED + 1))
else
    echo -e "  ${RED}✗${NC} Failed to detect undefined variable 'countr'"
    FAILED=$((FAILED + 1))
fi

# Test 2: Check if suggestion for 'counter' is provided
echo -e "${YELLOW}Test 2:${NC} Checking for 'Did you mean' suggestion for 'counter'"
if echo "$output" | grep -qi "did you mean.*counter"; then
    echo -e "  ${GREEN}✓${NC} Suggested 'counter' as correction"
    PASSED=$((PASSED + 1))
else
    echo -e "  ${RED}✗${NC} Failed to suggest 'counter'"
    FAILED=$((FAILED + 1))
fi

# Test 3: Check if error is detected for 'temperatur'
echo -e "${YELLOW}Test 3:${NC} Checking for undefined variable 'temperatur'"
if echo "$output" | grep -qi "temperatur"; then
    echo -e "  ${GREEN}✓${NC} Undefined variable 'temperatur' detected"
    PASSED=$((PASSED + 1))
else
    echo -e "  ${RED}✗${NC} Failed to detect undefined variable 'temperatur'"
    FAILED=$((FAILED + 1))
fi

# Test 4: Check if suggestion for 'temperature' is provided
echo -e "${YELLOW}Test 4:${NC} Checking for 'Did you mean' suggestion for 'temperature'"
if echo "$output" | grep -qi "did you mean.*temperature"; then
    echo -e "  ${GREEN}✓${NC} Suggested 'temperature' as correction"
    PASSED=$((PASSED + 1))
else
    echo -e "  ${RED}✗${NC} Failed to suggest 'temperature'"
    FAILED=$((FAILED + 1))
fi

# Test 5: Check if hints are present in output
echo -e "${YELLOW}Test 5:${NC} Checking for 'Hint:' label in output"
if echo "$output" | grep -qi "hint:"; then
    echo -e "  ${GREEN}✓${NC} Output includes 'Hint:' label"
    PASSED=$((PASSED + 1))
else
    echo -e "  ${RED}✗${NC} No 'Hint:' label found"
    FAILED=$((FAILED + 1))
fi

echo ""
echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}Summary${NC}"
echo -e "${BLUE}========================================${NC}"
TOTAL=$((PASSED + FAILED))
echo -e "Total tests: $TOTAL"
echo -e "${GREEN}Passed: $PASSED${NC}"
echo -e "${RED}Failed: $FAILED${NC}"
echo ""

if [ $FAILED -eq 0 ]; then
    echo -e "${GREEN}All tests passed! 'Did you mean?' is working correctly!${NC}"
    exit 0
else
    echo -e "${RED}Some tests failed. 'Did you mean?' may need adjustment.${NC}"
    exit 1
fi
