#!/bin/bash
# Comprehensive Test Runner for MiniC Compiler
# Tests all categories and generates detailed report

COMPILER="../mccomp"
BASE_DIR="$(dirname "$0")"

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Counters
TOTAL=0
PASSED=0
FAILED=0

# Arrays to store results
declare -a PASSING_TESTS
declare -a FAILING_TESTS

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}MiniC Comprehensive Test Suite${NC}"
echo -e "${BLUE}========================================${NC}"
echo ""

# Function to run a single test
run_test() {
    local test_file=$1
    local should_pass=$2  # "COMPILE" or "FAIL"
    local test_name=$(basename "$test_file" .c)
    local category=$(basename $(dirname "$test_file"))

    TOTAL=$((TOTAL + 1))

    # Compile the test
    local output
    output=$(timeout 5 $COMPILER "$test_file" 2>&1)
    local exit_code=$?

    local test_passed=false

    if [ "$should_pass" == "COMPILE" ]; then
        # Test should compile successfully
        if [ $exit_code -eq 0 ]; then
            echo -e "${GREEN}[PASS]${NC} $category/$test_name - Compiled successfully"
            PASSED=$((PASSED + 1))
            PASSING_TESTS+=("$category/$test_name: compiled OK")
            test_passed=true
        else
            echo -e "${RED}[FAIL]${NC} $category/$test_name - Should compile but failed"
            echo "       Error: $(echo "$output" | grep -i "error" | head -n 1)"
            FAILED=$((FAILED + 1))
            FAILING_TESTS+=("$category/$test_name: should compile but failed")
        fi
    else
        # Test should fail compilation
        if [ $exit_code -ne 0 ]; then
            echo -e "${GREEN}[PASS]${NC} $category/$test_name - Correctly rejected"
            PASSED=$((PASSED + 1))
            PASSING_TESTS+=("$category/$test_name: correctly rejected")
            test_passed=true
        else
            echo -e "${RED}[FAIL]${NC} $category/$test_name - Should fail but compiled"
            FAILED=$((FAILED + 1))
            FAILING_TESTS+=("$category/$test_name: should fail but compiled")
        fi
    fi

    return 0
}

# Function to determine if test should pass or fail
should_compile() {
    local filename=$1
    # Tests with "error" in filename should fail
    if [[ $filename == *"error"* ]]; then
        echo "FAIL"
    else
        echo "COMPILE"
    fi
}

echo -e "${BLUE}Running Type System Tests...${NC}"
for test in "$BASE_DIR/type_tests"/*.c; do
    run_test "$test" "$(should_compile $(basename "$test"))"
done
echo ""

echo -e "${BLUE}Running Scope Tests...${NC}"
for test in "$BASE_DIR/scope_tests"/*.c; do
    run_test "$test" "$(should_compile $(basename "$test"))"
done
echo ""

echo -e "${BLUE}Running Function Tests...${NC}"
for test in "$BASE_DIR/func_tests"/*.c; do
    run_test "$test" "$(should_compile $(basename "$test"))"
done
echo ""

echo -e "${BLUE}Running Expression Tests...${NC}"
for test in "$BASE_DIR/expr_tests"/*.c; do
    run_test "$test" "COMPILE"  # All expression tests should compile
done
echo ""

echo -e "${BLUE}Running Control Flow Tests...${NC}"
for test in "$BASE_DIR/ctrl_tests"/*.c; do
    run_test "$test" "COMPILE"  # All control flow tests should compile
done
echo ""

echo -e "${BLUE}Running Error Detection Tests...${NC}"
for test in "$BASE_DIR/error_tests"/*.c; do
    # All error tests should fail except specific ones
    if [[ $(basename "$test") == "10_missing_return.c" ]]; then
        run_test "$test" "COMPILE"  # Compiler adds default return
    else
        run_test "$test" "FAIL"
    fi
done
echo ""

# Print summary
echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}Test Summary${NC}"
echo -e "${BLUE}========================================${NC}"
echo -e "Total tests: $TOTAL"
echo -e "${GREEN}Passed: $PASSED${NC}"
echo -e "${RED}Failed: $FAILED${NC}"
echo ""

if [ $TOTAL -gt 0 ]; then
    SUCCESS_RATE=$((PASSED * 100 / TOTAL))
    echo -e "Success rate: ${SUCCESS_RATE}%"
fi
echo ""

# Report failures if any
if [ $FAILED -gt 0 ]; then
    echo -e "${RED}========================================${NC}"
    echo -e "${RED}Failed Tests${NC}"
    echo -e "${RED}========================================${NC}"
    for failure in "${FAILING_TESTS[@]}"; do
        echo -e "${RED}  - $failure${NC}"
    done
    echo ""
    exit 1
else
    echo -e "${GREEN}All tests passed!${NC}"
    exit 0
fi
