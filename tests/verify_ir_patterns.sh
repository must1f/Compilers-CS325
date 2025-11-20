#!/bin/bash

# IR Pattern Verification Script
# Compares generated IR against expected patterns based on lecturer tests

COMPILER="../../mccomp"
if [ ! -f "$COMPILER" ]; then
    COMPILER="../mccomp"
fi

VALID_DIR="negative_tests/reference_valid"
LECTURER_TESTS=("addition" "factorial" "fibonacci" "void" "while")

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
NC='\033[0m'

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}IR Pattern Verification${NC}"
echo -e "${BLUE}========================================${NC}"
echo ""

TOTAL=0
PASSED=0
FAILED=0

# Function to check IR pattern
check_ir_pattern() {
    local test_file=$1
    local test_name=$(basename "$test_file" .c)
    local expected_patterns=("${@:2}")
    
    TOTAL=$((TOTAL + 1))
    
    # Compile
    timeout 5 $COMPILER "$test_file" >/dev/null 2>&1
    if [ $? -ne 0 ]; then
        echo -e "${RED}[FAIL]${NC} $test_name - Compilation failed"
        FAILED=$((FAILED + 1))
        return 1
    fi
    
    if [ ! -f "output.ll" ]; then
        echo -e "${RED}[FAIL]${NC} $test_name - No IR output generated"
        FAILED=$((FAILED + 1))
        return 1
    fi
    
    # Check for expected patterns
    local all_found=true
    for pattern in "${expected_patterns[@]}"; do
        if ! grep -q "$pattern" output.ll; then
            echo -e "${YELLOW}[WARNING]${NC} $test_name - Missing pattern: $pattern"
            all_found=false
        fi
    done
    
    if $all_found; then
        echo -e "${GREEN}[PASS]${NC} $test_name - All IR patterns present"
        PASSED=$((PASSED + 1))
    else
        echo -e "${YELLOW}[PARTIAL]${NC} $test_name - Some patterns missing"
        FAILED=$((FAILED + 1))
    fi
    
    # Clean up
    rm -f output.ll
}

# Test valid reference files
echo -e "${BLUE}Verifying Valid Reference Tests...${NC}"

if [ -f "$VALID_DIR/int_to_float_assign.c" ]; then
    check_ir_pattern "$VALID_DIR/int_to_float_assign.c" "sitofp" "i32.*to float"
fi

if [ -f "$VALID_DIR/arg_widening.c" ]; then
    check_ir_pattern "$VALID_DIR/arg_widening.c" "sitofp" "define.*float"
fi

if [ -f "$VALID_DIR/comparison_to_bool.c" ]; then
    check_ir_pattern "$VALID_DIR/comparison_to_bool.c" "icmp" "i1"
fi

if [ -f "$VALID_DIR/all_widening.c" ]; then
    check_ir_pattern "$VALID_DIR/all_widening.c" "sitofp" "zext"
fi

if [ -f "$VALID_DIR/array_operations.c" ]; then
    check_ir_pattern "$VALID_DIR/array_operations.c" "getelementptr" "alloca.*\[.*x"
fi

if [ -f "$VALID_DIR/proper_function_calls.c" ]; then
    check_ir_pattern "$VALID_DIR/proper_function_calls.c" "call.*@" "define.*@"
fi

echo ""

# Verify lecturer tests still compile correctly
echo -e "${BLUE}Verifying Lecturer Reference Tests...${NC}"

for test_dir in "${LECTURER_TESTS[@]}"; do
    if [ -d "$test_dir" ]; then
        test_file=$(find "$test_dir" -name "*.c" | head -n 1)
        if [ -f "$test_file" ]; then
            TOTAL=$((TOTAL + 1))
            timeout 5 $COMPILER "$test_file" >/dev/null 2>&1
            if [ $? -eq 0 ] && [ -f "output.ll" ]; then
                # Check for basic IR structure
                if grep -q "define.*@" output.ll && grep -q "ret" output.ll; then
                    echo -e "${GREEN}[PASS]${NC} $test_dir - Valid IR generated"
                    PASSED=$((PASSED + 1))
                else
                    echo -e "${RED}[FAIL]${NC} $test_dir - IR incomplete"
                    FAILED=$((FAILED + 1))
                fi
                rm -f output.ll
            else
                echo -e "${RED}[FAIL]${NC} $test_dir - Compilation failed"
                FAILED=$((FAILED + 1))
            fi
        fi
    fi
done

echo ""
echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}IR Verification Summary${NC}"
echo -e "${BLUE}========================================${NC}"
echo -e "Total IR tests: $TOTAL"
echo -e "${GREEN}Passed: $PASSED${NC}"
echo -e "${RED}Failed: $FAILED${NC}"
echo ""

if [ $TOTAL -gt 0 ]; then
    SUCCESS_RATE=$((PASSED * 100 / TOTAL))
    echo -e "IR generation success rate: ${SUCCESS_RATE}%"
fi

[ $FAILED -eq 0 ]

