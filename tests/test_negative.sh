#!/bin/bash

# Comprehensive negative test driver for MiniC compiler
# Tests that compiler correctly rejects invalid programs

COMPILER="../../mccomp"
if [ ! -f "$COMPILER" ]; then
    COMPILER="../mccomp"
fi

NEGATIVE_DIR="negative_tests"
SYNTAX_DIR="$NEGATIVE_DIR/syntax_errors"
SEMANTIC_DIR="$NEGATIVE_DIR/semantic_errors"
SCOPE_DIR="$NEGATIVE_DIR/scope_errors"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Counters
TOTAL=0
PASSED=0
FAILED=0
PARTIAL=0

# Test results arrays
declare -a FAILED_TESTS
declare -a PARTIAL_TESTS

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}MiniC Compiler Negative Test Suite${NC}"
echo -e "${BLUE}========================================${NC}"
echo ""

# Function to run a single test
run_test() {
    local test_file=$1
    local expected_error_type=$2
    local test_name=$(basename "$test_file" .c)
    
    TOTAL=$((TOTAL + 1))

    # Run compiler and capture output
    local output
    output=$(timeout 5 $COMPILER "$test_file" 2>&1)
    local exit_code=$?
    
    # Compiler should fail (non-zero exit code)
    if [ $exit_code -eq 0 ]; then
        echo -e "${RED}[FAIL]${NC} $test_name - Compiler accepted invalid code!"
        FAILED=$((FAILED + 1))
        FAILED_TESTS+=("$test_name: accepted invalid code")
        return 1
    fi
    
    # Check if error message contains expected error type
    if echo "$output" | grep -qi "$expected_error_type"; then
        echo -e "${GREEN}[PASS]${NC} $test_name - Correctly rejected with $expected_error_type"
        PASSED=$((PASSED + 1))
        return 0
    else
        # Rejected but with wrong error type
        actual_error=$(echo "$output" | grep -i "error" | head -n 1)
        echo -e "${YELLOW}[PARTIAL]${NC} $test_name - Rejected but expected '$expected_error_type', got: $actual_error"
        PARTIAL=$((PARTIAL + 1))
        PARTIAL_TESTS+=("$test_name: expected '$expected_error_type'")
        return 2
    fi
}

# Run syntax error tests
echo -e "${BLUE}Testing Syntax Errors...${NC}"
for test in $SYNTAX_DIR/*.c; do
    if [ -f "$test" ]; then
        run_test "$test" "Syntax Error"
    fi
done
echo ""

# Run semantic type error tests
echo -e "${BLUE}Testing Semantic Type Errors...${NC}"
for test in $SEMANTIC_DIR/*.c; do
    if [ -f "$test" ]; then
        # Most semantic tests should report Type Error
        run_test "$test" "Type Error\|Semantic Error"
    fi
done
echo ""

# Run scope error tests
echo -e "${BLUE}Testing Scope Errors...${NC}"
for test in $SCOPE_DIR/*.c; do
    if [ -f "$test" ]; then
        run_test "$test" "Scope Error\|undefined\|redeclaration"
    fi
done
echo ""

# Print summary
echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}Test Summary${NC}"
echo -e "${BLUE}========================================${NC}"
echo -e "Total tests: $TOTAL"
echo -e "${GREEN}Passed: $PASSED${NC} (correctly rejected with right error)"
echo -e "${YELLOW}Partial: $PARTIAL${NC} (rejected but wrong error type)"
echo -e "${RED}Failed: $FAILED${NC} (incorrectly accepted invalid code)"
echo ""

# Calculate success rate
if [ $TOTAL -gt 0 ]; then
    SUCCESS_RATE=$((PASSED * 100 / TOTAL))
    DETECTION_RATE=$(( (PASSED + PARTIAL) * 100 / TOTAL ))
    echo -e "Success rate (correct error): ${SUCCESS_RATE}%"
    echo -e "Detection rate (any error): ${DETECTION_RATE}%"
fi
echo ""

# Report failures in detail
if [ $FAILED -gt 0 ]; then
    echo -e "${RED}========================================${NC}"
    echo -e "${RED}CRITICAL FAILURES (Accepted Invalid Code)${NC}"
    echo -e "${RED}========================================${NC}"
    for failure in "${FAILED_TESTS[@]}"; do
        echo -e "${RED}  - $failure${NC}"
    done
    echo ""
fi

# Report partial matches
if [ $PARTIAL -gt 0 ]; then
    echo -e "${YELLOW}========================================${NC}"
    echo -e "${YELLOW}Partial Matches (Wrong Error Type)${NC}"
    echo -e "${YELLOW}========================================${NC}"
    for partial in "${PARTIAL_TESTS[@]}"; do
        echo -e "${YELLOW}  - $partial${NC}"
    done
    echo ""
fi

# Exit with appropriate code
if [ $FAILED -gt 0 ]; then
    exit 1
elif [ $PARTIAL -gt 0 ]; then
    exit 2
else
    exit 0
fi

