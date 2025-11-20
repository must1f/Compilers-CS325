#!/bin/bash

# Error Message Quality Test
# Verifies that error messages are helpful and include necessary information

COMPILER="../../mccomp"
if [ ! -f "$COMPILER" ]; then
    COMPILER="../mccomp"
fi

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}Error Message Quality Assessment${NC}"
echo -e "${BLUE}========================================${NC}"
echo ""

TOTAL_CHECKS=0
PASSED_CHECKS=0

# Function to check error quality
check_error_quality() {
    local test_file=$1
    local test_name=$2
    local check_type=$3
    
    TOTAL_CHECKS=$((TOTAL_CHECKS + 1))
    
    output=$(timeout 5 $COMPILER "$test_file" 2>&1)
    
    case $check_type in
        "has_line_number")
            if echo "$output" | grep -qE "line [0-9]+|[0-9]+:[0-9]+"; then
                echo -e "  ${GREEN}✓${NC} $test_name has line number"
                PASSED_CHECKS=$((PASSED_CHECKS + 1))
            else
                echo -e "  ${RED}✗${NC} $test_name missing line number"
            fi
            ;;
        "has_error_type")
            if echo "$output" | grep -qiE "error|Error"; then
                echo -e "  ${GREEN}✓${NC} $test_name shows error type"
                PASSED_CHECKS=$((PASSED_CHECKS + 1))
            else
                echo -e "  ${RED}✗${NC} $test_name missing error type label"
            fi
            ;;
        "has_context")
            if echo "$output" | grep -qiE "function|context|in|at"; then
                echo -e "  ${GREEN}✓${NC} $test_name provides context"
                PASSED_CHECKS=$((PASSED_CHECKS + 1))
            else
                echo -e "  ${YELLOW}~${NC} $test_name could use more context"
            fi
            ;;
        "has_variable_name")
            # Check if error mentions a variable/identifier name
            if echo "$output" | grep -qE "'[a-zA-Z_][a-zA-Z0-9_]*'"; then
                echo -e "  ${GREEN}✓${NC} $test_name mentions variable/identifier name"
                PASSED_CHECKS=$((PASSED_CHECKS + 1))
            else
                echo -e "  ${YELLOW}~${NC} $test_name could mention specific names"
            fi
            ;;
        "has_color")
            # Check for ANSI color codes
            if echo "$output" | grep -qE "\[0;3[0-9]m|\[1;3[0-9]m"; then
                echo -e "  ${GREEN}✓${NC} $test_name uses color coding"
                PASSED_CHECKS=$((PASSED_CHECKS + 1))
            else
                echo -e "  ${YELLOW}~${NC} $test_name no color coding"
            fi
            ;;
    esac
}

# Test a few key error cases
echo -e "${BLUE}Testing undefined variable error quality...${NC}"
cat > /tmp/test_undefined.c << 'EOF'
int main() {
    int x;
    return y;
}
EOF
check_error_quality "/tmp/test_undefined.c" "undefined_var" "has_line_number"
check_error_quality "/tmp/test_undefined.c" "undefined_var" "has_error_type"
check_error_quality "/tmp/test_undefined.c" "undefined_var" "has_variable_name"
check_error_quality "/tmp/test_undefined.c" "undefined_var" "has_color"
echo ""

echo -e "${BLUE}Testing type error message quality...${NC}"
cat > /tmp/test_type.c << 'EOF'
int main() {
    int x;
    float y;
    y = 3.14;
    x = y;
    return x;
}
EOF
check_error_quality "/tmp/test_type.c" "type_error" "has_line_number"
check_error_quality "/tmp/test_type.c" "type_error" "has_error_type"
check_error_quality "/tmp/test_type.c" "type_error" "has_context"
check_error_quality "/tmp/test_type.c" "type_error" "has_color"
echo ""

echo -e "${BLUE}Testing syntax error message quality...${NC}"
cat > /tmp/test_syntax.c << 'EOF'
int main() {
    int x
    return x;
}
EOF
check_error_quality "/tmp/test_syntax.c" "syntax_error" "has_line_number"
check_error_quality "/tmp/test_syntax.c" "syntax_error" "has_error_type"
check_error_quality "/tmp/test_syntax.c" "syntax_error" "has_color"
echo ""

# Test error quality on actual test files
echo -e "${BLUE}Checking negative test suite error messages...${NC}"
NEGATIVE_DIR="negative_tests"

# Sample a few tests from each category
for test in "$NEGATIVE_DIR/syntax_errors/missing_semicolon.c" \
            "$NEGATIVE_DIR/semantic_errors/float_to_int_assign.c" \
            "$NEGATIVE_DIR/scope_errors/undefined_variable.c"; do
    if [ -f "$test" ]; then
        test_name=$(basename "$test" .c)
        echo "  Checking $test_name..."
        check_error_quality "$test" "$test_name" "has_line_number"
        check_error_quality "$test" "$test_name" "has_error_type"
    fi
done

echo ""
echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}Error Message Quality Summary${NC}"
echo -e "${BLUE}========================================${NC}"
echo -e "Total quality checks: $TOTAL_CHECKS"
echo -e "${GREEN}Passed: $PASSED_CHECKS${NC}"
echo -e "${RED}Failed: $((TOTAL_CHECKS - PASSED_CHECKS))${NC}"
echo ""

if [ $TOTAL_CHECKS -gt 0 ]; then
    QUALITY_SCORE=$((PASSED_CHECKS * 100 / TOTAL_CHECKS))
    echo -e "Error message quality score: ${QUALITY_SCORE}%"
    echo ""
    
    if [ $QUALITY_SCORE -ge 80 ]; then
        echo -e "${GREEN}Excellent error message quality!${NC}"
    elif [ $QUALITY_SCORE -ge 60 ]; then
        echo -e "${YELLOW}Good error messages, room for improvement${NC}"
    else
        echo -e "${RED}Error messages need improvement${NC}"
    fi
fi

# Cleanup
rm -f /tmp/test_undefined.c /tmp/test_type.c /tmp/test_syntax.c

