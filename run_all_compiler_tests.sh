#!/bin/bash
# Master Test Runner for MiniC Compiler
# Runs all test suites and provides comprehensive summary

set -e  # Exit on error (we'll handle it ourselves)

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
BOLD='\033[1m'
NC='\033[0m' # No Color

# Counters
TOTAL_SUITES=0
PASSED_SUITES=0
FAILED_SUITES=0

echo -e "${BOLD}${BLUE}========================================${NC}"
echo -e "${BOLD}${BLUE}MiniC Compiler - Complete Test Suite${NC}"
echo -e "${BOLD}${BLUE}========================================${NC}"
echo ""

# Check if compiler exists
if [ ! -f "./mccomp" ]; then
    echo -e "${RED}ERROR: mccomp compiler not found!${NC}"
    echo "Please compile the compiler first with: make"
    exit 1
fi

echo -e "${CYAN}Compiler found: ./mccomp${NC}"
echo ""

# Array to store results
declare -a SUITE_RESULTS

# Function to run a test suite
run_suite() {
    local suite_name=$1
    local suite_script=$2
    local suite_dir=$3

    TOTAL_SUITES=$((TOTAL_SUITES + 1))

    echo -e "${BOLD}${BLUE}========================================${NC}"
    echo -e "${BOLD}${BLUE}Running: $suite_name${NC}"
    echo -e "${BOLD}${BLUE}========================================${NC}"

    if [ ! -f "$suite_script" ]; then
        echo -e "${YELLOW}SKIPPED: Test script not found: $suite_script${NC}"
        SUITE_RESULTS+=("$suite_name: SKIPPED (script not found)")
        echo ""
        return
    fi

    # Make script executable
    chmod +x "$suite_script"

    # Run the test suite
    local start_time=$(date +%s)
    if (cd "$suite_dir" && bash "$(basename "$suite_script")"); then
        local end_time=$(date +%s)
        local duration=$((end_time - start_time))
        echo -e "${GREEN}✓ $suite_name PASSED${NC} (${duration}s)"
        PASSED_SUITES=$((PASSED_SUITES + 1))
        SUITE_RESULTS+=("$suite_name: PASSED (${duration}s)")
    else
        local end_time=$(date +%s)
        local duration=$((end_time - start_time))
        echo -e "${RED}✗ $suite_name FAILED${NC} (${duration}s)"
        FAILED_SUITES=$((FAILED_SUITES + 1))
        SUITE_RESULTS+=("$suite_name: FAILED (${duration}s)")
    fi
    echo ""
}

# 1. Run Comprehensive Edge Case Tests (60 tests)
if [ -d "comprehensive_tests" ]; then
    run_suite "Comprehensive Edge Cases (60 tests)" \
              "comprehensive_tests/run_all_tests.sh" \
              "comprehensive_tests"
else
    echo -e "${YELLOW}Comprehensive tests directory not found, skipping...${NC}"
    echo ""
fi

# 2. Run Lecturer-Provided Tests
if [ -d "tests" ]; then
    echo -e "${BOLD}${BLUE}========================================${NC}"
    echo -e "${BOLD}${BLUE}Running: Lecturer-Provided Tests${NC}"
    echo -e "${BOLD}${BLUE}========================================${NC}"

    TOTAL_SUITES=$((TOTAL_SUITES + 1))

    # Manually run basic lecturer tests
    LECTURER_PASS=0
    LECTURER_TOTAL=0

    # Core tests that should always work
    for test_name in addition factorial fibonacci void while; do
        test_dir="tests/$test_name"
        if [ -d "$test_dir" ]; then
            test_file="$test_dir/${test_name}.c"

            if [ -f "$test_file" ]; then
                LECTURER_TOTAL=$((LECTURER_TOTAL + 1))
                echo -n "Testing $test_name... "

                if timeout 5 ./mccomp "$test_file" > /dev/null 2>&1; then
                    echo -e "${GREEN}PASS${NC}"
                    LECTURER_PASS=$((LECTURER_PASS + 1))
                else
                    echo -e "${RED}FAIL${NC}"
                fi
            fi
        fi
    done

    # Additional tests if they exist
    for test_name in array_addition cosine global_array leap matrix_multiplication null palindrome pi prime simple_return; do
        test_dir="tests/$test_name"
        if [ -d "$test_dir" ]; then
            test_file="$test_dir/${test_name}.c"

            if [ -f "$test_file" ]; then
                LECTURER_TOTAL=$((LECTURER_TOTAL + 1))
                echo -n "Testing $test_name... "

                if timeout 5 ./mccomp "$test_file" > /dev/null 2>&1; then
                    echo -e "${GREEN}PASS${NC}"
                    LECTURER_PASS=$((LECTURER_PASS + 1))
                else
                    echo -e "${RED}FAIL${NC}"
                fi
            fi
        fi
    done

    echo ""
    echo "Results: $LECTURER_PASS/$LECTURER_TOTAL passed"

    if [ $LECTURER_PASS -eq $LECTURER_TOTAL ]; then
        echo -e "${GREEN}✓ Lecturer-Provided Tests PASSED${NC}"
        PASSED_SUITES=$((PASSED_SUITES + 1))
        SUITE_RESULTS+=("Lecturer-Provided Tests: PASSED ($LECTURER_PASS/$LECTURER_TOTAL)")
    else
        echo -e "${RED}✗ Lecturer-Provided Tests FAILED${NC}"
        FAILED_SUITES=$((FAILED_SUITES + 1))
        SUITE_RESULTS+=("Lecturer-Provided Tests: FAILED ($LECTURER_PASS/$LECTURER_TOTAL)")
    fi
    echo ""
fi

# 3. Run Negative Tests (if desired - these validate error detection)
if [ -d "tests/negative_tests" ] && [ -f "tests/run_negative_tests.sh" ]; then
    run_suite "Negative Tests (Error Detection)" \
              "tests/run_negative_tests.sh" \
              "tests"
fi

# Print overall summary
echo -e "${BOLD}${BLUE}========================================${NC}"
echo -e "${BOLD}${BLUE}OVERALL TEST SUMMARY${NC}"
echo -e "${BOLD}${BLUE}========================================${NC}"
echo ""

for result in "${SUITE_RESULTS[@]}"; do
    if [[ $result == *"PASSED"* ]]; then
        echo -e "${GREEN}✓ $result${NC}"
    elif [[ $result == *"FAILED"* ]]; then
        echo -e "${RED}✗ $result${NC}"
    else
        echo -e "${YELLOW}⊘ $result${NC}"
    fi
done

echo ""
echo -e "${BOLD}Test Suites Summary:${NC}"
echo -e "  Total suites run: $TOTAL_SUITES"
echo -e "  ${GREEN}Passed: $PASSED_SUITES${NC}"
echo -e "  ${RED}Failed: $FAILED_SUITES${NC}"

if [ $TOTAL_SUITES -gt 0 ]; then
    SUCCESS_RATE=$((PASSED_SUITES * 100 / TOTAL_SUITES))
    echo -e "  Success rate: ${SUCCESS_RATE}%"
fi

echo ""
echo -e "${BOLD}${BLUE}========================================${NC}"

# Exit with appropriate code
if [ $FAILED_SUITES -eq 0 ]; then
    echo -e "${BOLD}${GREEN}ALL TEST SUITES PASSED!${NC}"
    echo -e "${BOLD}${BLUE}========================================${NC}"
    exit 0
else
    echo -e "${BOLD}${RED}SOME TEST SUITES FAILED${NC}"
    echo -e "${BOLD}${BLUE}========================================${NC}"
    exit 1
fi
