#!/bin/bash
# Comprehensive MiniC Test Suite Generator
# Generates extensive edge case tests for all language features

BASE_DIR="$(dirname "$0")"
TYPE_DIR="$BASE_DIR/type_tests"
SCOPE_DIR="$BASE_DIR/scope_tests"
FUNC_DIR="$BASE_DIR/func_tests"
EXPR_DIR="$BASE_DIR/expr_tests"
CTRL_DIR="$BASE_DIR/ctrl_tests"
ERROR_DIR="$BASE_DIR/error_tests"

echo "Generating comprehensive MiniC test suite..."

#============================================================================
# TYPE TESTS - Widening/Narrowing Conversions
#============================================================================
echo "Generating type tests..."

# Test 01: bool→int widening
cat > "$TYPE_DIR/01_bool_to_int_widening.c" << 'EOF'
// TEST: bool→int widening (SHOULD COMPILE)
int main() {
    bool b;
    int i;
    b = true;
    i = b;  // bool→int widening: ALLOWED
    return i;
}
EOF

# Test 02: int→float widening
cat > "$TYPE_DIR/02_int_to_float_widening.c" << 'EOF'
// TEST: int→float widening (SHOULD COMPILE)
int main() {
    int i;
    float f;
    i = 42;
    f = i;  // int→float widening: ALLOWED
    return 0;
}
EOF

# Test 03: bool→float widening
cat > "$TYPE_DIR/03_bool_to_float_widening.c" << 'EOF'
// TEST: bool→float widening (SHOULD COMPILE)
int main() {
    bool b;
    float f;
    b = true;
    f = b;  // bool→float widening (via int): ALLOWED
    return 0;
}
EOF

# Test 04: float→int narrowing ERROR
cat > "$TYPE_DIR/04_float_to_int_narrowing_error.c" << 'EOF'
// TEST: float→int narrowing (SHOULD FAIL)
int main() {
    float f;
    int i;
    f = 3.14;
    i = f;  // float→int narrowing: ERROR
    return 0;
}
EOF

# Test 05: int→bool narrowing ERROR (outside conditional)
cat > "$TYPE_DIR/05_int_to_bool_narrowing_error.c" << 'EOF'
// TEST: int→bool narrowing in assignment (SHOULD FAIL)
int main() {
    int i;
    bool b;
    i = 5;
    b = i;  // int→bool narrowing: ERROR
    return 0;
}
EOF

# Test 06: Conditional narrowing OK
cat > "$TYPE_DIR/06_conditional_narrowing_ok.c" << 'EOF'
// TEST: Narrowing in conditionals (SHOULD COMPILE)
int main() {
    int i;
    float f;
    i = 5;
    f = 3.14;

    if (i) {     // int→bool in conditional: OK
        i = 1;
    }

    if (f) {     // float→bool in conditional: OK
        i = 2;
    }

    return i;
}
EOF

# Test 07: Logical operators with narrowing
cat > "$TYPE_DIR/07_logical_narrowing_ok.c" << 'EOF'
// TEST: Logical operators allow narrowing (SHOULD COMPILE)
int main() {
    int x;
    int y;
    bool result;

    x = 5;
    y = 10;

    result = x && y;  // int→bool for &&: OK
    result = x || y;  // int→bool for ||: OK
    result = !x;      // int→bool for !: OK

    return 0;
}
EOF

# Test 08: Modulo on float ERROR
cat > "$TYPE_DIR/08_modulo_float_error.c" << 'EOF'
// TEST: Modulo requires integers (SHOULD FAIL)
int main() {
    float x;
    float y;
    float result;

    x = 5.5;
    y = 2.2;
    result = x % y;  // Modulo on float: ERROR

    return 0;
}
EOF

# Test 09: Arithmetic on bool ERROR
cat > "$TYPE_DIR/09_arithmetic_bool_error.c" << 'EOF'
// TEST: Arithmetic operators require numeric types (SHOULD FAIL)
int main() {
    bool x;
    bool y;
    int result;

    x = true;
    y = false;
    result = x + y;  // Arithmetic on bool: ERROR

    return 0;
}
EOF

# Test 10: Mixed type arithmetic with automatic widening
cat > "$TYPE_DIR/10_mixed_arithmetic_widening.c" << 'EOF'
// TEST: Mixed int/float arithmetic (SHOULD COMPILE)
int main() {
    int i;
    float f;
    float result;

    i = 5;
    f = 3.14;
    result = i + f;  // int→float widening for arithmetic: OK

    return 0;
}
EOF

#============================================================================
# SCOPE TESTS - Global/Local, Shadowing, Use-before-declaration
#============================================================================
echo "Generating scope tests..."

# Test 01: Global single declaration
cat > "$SCOPE_DIR/01_global_single_decl.c" << 'EOF'
// TEST: Single global declaration (SHOULD COMPILE)
int global_var;

int main() {
    global_var = 42;
    return global_var;
}
EOF

# Test 02: Global redeclaration ERROR
cat > "$SCOPE_DIR/02_global_redecl_error.c" << 'EOF'
// TEST: Global redeclaration (SHOULD FAIL)
int global_var;
int global_var;  // ERROR: redeclaration

int main() {
    return 0;
}
EOF

# Test 03: Local shadowing global OK
cat > "$SCOPE_DIR/03_local_shadows_global.c" << 'EOF'
// TEST: Local shadowing global (SHOULD COMPILE)
int x;

int main() {
    int x;  // Shadows global: OK
    x = 42;
    return x;
}
EOF

# Test 04: Multiple shadow levels
cat > "$SCOPE_DIR/04_multiple_shadow_levels.c" << 'EOF'
// TEST: Multiple shadow levels (SHOULD COMPILE)
int x;

int main() {
    int x;
    x = 1;

    {
        int x;
        x = 2;

        {
            int x;
            x = 3;
        }
    }

    return x;
}
EOF

# Test 05: Duplicate in same scope ERROR
cat > "$SCOPE_DIR/05_duplicate_same_scope_error.c" << 'EOF'
// TEST: Duplicate in same scope (SHOULD FAIL)
int main() {
    int x;
    int x;  // ERROR: duplicate in same scope
    return 0;
}
EOF

# Test 06: Use before declaration ERROR
cat > "$SCOPE_DIR/06_use_before_decl_error.c" << 'EOF'
// TEST: Use before declaration (SHOULD FAIL)
int main() {
    x = 42;  // ERROR: x not declared yet
    int x;
    return x;
}
EOF

# Test 07: Forward function reference OK
cat > "$SCOPE_DIR/07_forward_function_ok.c" << 'EOF'
// TEST: Forward function reference (SHOULD COMPILE)
int helper();

int main() {
    return helper();
}

int helper() {
    return 42;
}
EOF

# Test 08: Undefined function ERROR
cat > "$SCOPE_DIR/08_undefined_function_error.c" << 'EOF'
// TEST: Undefined function (SHOULD FAIL)
int main() {
    return undefined_func();  // ERROR: function not declared
}
EOF

# Test 09: Variable out of scope ERROR
cat > "$SCOPE_DIR/09_out_of_scope_error.c" << 'EOF'
// TEST: Variable out of scope (SHOULD FAIL)
int main() {
    {
        int x;
        x = 42;
    }
    return x;  // ERROR: x out of scope
}
EOF

# Test 10: Function redefinition ERROR
cat > "$SCOPE_DIR/10_function_redef_error.c" << 'EOF'
// TEST: Function redefinition (SHOULD FAIL)
int foo() {
    return 1;
}

int foo() {  // ERROR: redefinition
    return 2;
}

int main() {
    return 0;
}
EOF

#============================================================================
# FUNCTION TESTS - Empty params, return types, argument checking
#============================================================================
echo "Generating function tests..."

# Test 01: Empty params means zero args
cat > "$FUNC_DIR/01_empty_params_zero_args.c" << 'EOF'
// TEST: foo() means exactly zero arguments (SHOULD COMPILE)
int no_args() {
    return 42;
}

int main() {
    return no_args();  // OK: zero arguments
}
EOF

# Test 02: Empty params with args ERROR
cat > "$FUNC_DIR/02_empty_params_with_args_error.c" << 'EOF'
// TEST: foo() with arguments (SHOULD FAIL)
int no_args() {
    return 42;
}

int main() {
    return no_args(5);  // ERROR: expects 0 args, got 1
}
EOF

# Test 03: Return type widening OK
cat > "$FUNC_DIR/03_return_widening_ok.c" << 'EOF'
// TEST: Return type widening (SHOULD COMPILE)
float get_float() {
    int x;
    x = 42;
    return x;  // int→float widening: OK
}

int main() {
    float f;
    f = get_float();
    return 0;
}
EOF

# Test 04: Return type narrowing ERROR
cat > "$FUNC_DIR/04_return_narrowing_error.c" << 'EOF'
// TEST: Return type narrowing (SHOULD FAIL)
int get_int() {
    float f;
    f = 3.14;
    return f;  // float→int narrowing: ERROR
}

int main() {
    return get_int();
}
EOF

# Test 05: Argument widening OK
cat > "$FUNC_DIR/05_arg_widening_ok.c" << 'EOF'
// TEST: Argument type widening (SHOULD COMPILE)
void takes_float(float f) {
    return;
}

int main() {
    int x;
    x = 42;
    takes_float(x);  // int→float widening: OK
    return 0;
}
EOF

# Test 06: Argument narrowing ERROR
cat > "$FUNC_DIR/06_arg_narrowing_error.c" << 'EOF'
// TEST: Argument type narrowing (SHOULD FAIL)
void takes_int(int i) {
    return;
}

int main() {
    float f;
    f = 3.14;
    takes_int(f);  // float→int narrowing: ERROR
    return 0;
}
EOF

# Test 07: Wrong argument count - too few
cat > "$FUNC_DIR/07_too_few_args_error.c" << 'EOF'
// TEST: Too few arguments (SHOULD FAIL)
int add(int a, int b) {
    return a + b;
}

int main() {
    return add(5);  // ERROR: expects 2 args, got 1
}
EOF

# Test 08: Wrong argument count - too many
cat > "$FUNC_DIR/08_too_many_args_error.c" << 'EOF'
// TEST: Too many arguments (SHOULD FAIL)
int add(int a, int b) {
    return a + b;
}

int main() {
    return add(5, 10, 15);  // ERROR: expects 2 args, got 3
}
EOF

# Test 09: Deep recursion
cat > "$FUNC_DIR/09_deep_recursion.c" << 'EOF'
// TEST: Deep recursion (SHOULD COMPILE)
int factorial(int n) {
    if (n <= 1) {
        return 1;
    }
    return n * factorial(n - 1);
}

int main() {
    return factorial(10);
}
EOF

# Test 10: Mutual recursion
cat > "$FUNC_DIR/10_mutual_recursion.c" << 'EOF'
// TEST: Mutual recursion (SHOULD COMPILE)
int even(int n);
int odd(int n);

int even(int n) {
    if (n == 0) {
        return 1;
    }
    return odd(n - 1);
}

int odd(int n) {
    if (n == 0) {
        return 0;
    }
    return even(n - 1);
}

int main() {
    return even(10);
}
EOF

#============================================================================
# EXPRESSION TESTS - Operators, precedence, type promotion
#============================================================================
echo "Generating expression tests..."

# Test 01: Operator precedence
cat > "$EXPR_DIR/01_operator_precedence.c" << 'EOF'
// TEST: Operator precedence (SHOULD COMPILE)
int main() {
    int result;
    result = 2 + 3 * 4;  // Should be 14, not 20
    result = 10 - 6 / 2;  // Should be 7, not 2
    return result;
}
EOF

# Test 02: Associativity - left to right
cat > "$EXPR_DIR/02_left_associativity.c" << 'EOF'
// TEST: Left associativity (SHOULD COMPILE)
int main() {
    int result;
    result = 10 - 5 - 2;  // Should be 3 (left-to-right)
    result = 20 / 4 / 2;  // Should be 2 (left-to-right)
    return result;
}
EOF

# Test 03: Unary operator chains
cat > "$EXPR_DIR/03_unary_chains.c" << 'EOF'
// TEST: Unary operator chains (SHOULD COMPILE)
int main() {
    int x;
    bool b;

    x = 5;
    x = -(-x);    // Double negation

    b = true;
    b = !!b;      // Double logical NOT

    return 0;
}
EOF

# Test 04: Comparison chaining
cat > "$EXPR_DIR/04_comparison_operators.c" << 'EOF'
// TEST: All comparison operators (SHOULD COMPILE)
int main() {
    int x;
    int y;
    bool result;

    x = 5;
    y = 10;

    result = x < y;
    result = x <= y;
    result = x > y;
    result = x >= y;
    result = x == y;
    result = x != y;

    return 0;
}
EOF

# Test 05: Logical operators
cat > "$EXPR_DIR/05_logical_operators.c" << 'EOF'
// TEST: Logical operators (SHOULD COMPILE)
int main() {
    bool a;
    bool b;
    bool result;

    a = true;
    b = false;

    result = a && b;
    result = a || b;
    result = !a;
    result = !(a && b);
    result = (!a) || (!b);

    return 0;
}
EOF

# Test 06: Short-circuit evaluation
cat > "$EXPR_DIR/06_short_circuit.c" << 'EOF'
// TEST: Short-circuit evaluation (SHOULD COMPILE)
int side_effect() {
    return 1;
}

int main() {
    int x;
    bool result;

    x = 0;
    result = x && side_effect();  // side_effect() not called

    x = 1;
    result = x || side_effect();  // side_effect() not called

    return 0;
}
EOF

# Test 07: Complex expression
cat > "$EXPR_DIR/07_complex_expression.c" << 'EOF'
// TEST: Complex mixed expression (SHOULD COMPILE)
int main() {
    int a;
    int b;
    int c;
    float result;

    a = 5;
    b = 10;
    c = 3;

    result = (a + b) * c / 2.0 - 1.5;

    return 0;
}
EOF

# Test 08: Unary minus on different types
cat > "$EXPR_DIR/08_unary_minus_types.c" << 'EOF'
// TEST: Unary minus on numeric types (SHOULD COMPILE)
int main() {
    int i;
    float f;

    i = -42;
    f = -3.14;

    i = -i;
    f = -f;

    return 0;
}
EOF

# Test 09: Parentheses override precedence
cat > "$EXPR_DIR/09_parentheses.c" << 'EOF'
// TEST: Parentheses override precedence (SHOULD COMPILE)
int main() {
    int result;

    result = (2 + 3) * 4;   // Should be 20
    result = 2 + (3 * 4);   // Should be 14
    result = (10 - 6) / 2;  // Should be 2

    return result;
}
EOF

# Test 10: Type promotion in binary operations
cat > "$EXPR_DIR/10_type_promotion.c" << 'EOF'
// TEST: Type promotion in binary ops (SHOULD COMPILE)
int main() {
    int i;
    float f;
    float result;

    i = 10;
    f = 3.14;

    result = i + f;   // i promoted to float
    result = f * i;   // i promoted to float
    result = i / f;   // i promoted to float

    return 0;
}
EOF

#============================================================================
# CONTROL FLOW TESTS - if/else, while, nested structures
#============================================================================
echo "Generating control flow tests..."

# Test 01: Simple if
cat > "$CTRL_DIR/01_simple_if.c" << 'EOF'
// TEST: Simple if without else (SHOULD COMPILE)
int main() {
    int x;
    x = 5;

    if (x > 0) {
        x = 10;
    }

    return x;
}
EOF

# Test 02: If-else
cat > "$CTRL_DIR/02_if_else.c" << 'EOF'
// TEST: If-else (SHOULD COMPILE)
int main() {
    int x;
    int result;

    x = 5;

    if (x > 10) {
        result = 1;
    } else {
        result = 0;
    }

    return result;
}
EOF

# Test 03: Nested if
cat > "$CTRL_DIR/03_nested_if.c" << 'EOF'
// TEST: Nested if statements (SHOULD COMPILE)
int main() {
    int x;
    int y;
    int result;

    x = 5;
    y = 10;
    result = 0;

    if (x > 0) {
        if (y > 0) {
            result = 1;
        } else {
            result = 2;
        }
    } else {
        result = 3;
    }

    return result;
}
EOF

# Test 04: Simple while
cat > "$CTRL_DIR/04_simple_while.c" << 'EOF'
// TEST: Simple while loop (SHOULD COMPILE)
int main() {
    int i;
    int sum;

    i = 0;
    sum = 0;

    while (i < 10) {
        sum = sum + i;
        i = i + 1;
    }

    return sum;
}
EOF

# Test 05: Nested while
cat > "$CTRL_DIR/05_nested_while.c" << 'EOF'
// TEST: Nested while loops (SHOULD COMPILE)
int main() {
    int i;
    int j;
    int sum;

    i = 0;
    sum = 0;

    while (i < 5) {
        j = 0;
        while (j < 5) {
            sum = sum + 1;
            j = j + 1;
        }
        i = i + 1;
    }

    return sum;
}
EOF

# Test 06: While with break equivalent (return)
cat > "$CTRL_DIR/06_while_early_return.c" << 'EOF'
// TEST: While with early return (SHOULD COMPILE)
int main() {
    int i;

    i = 0;
    while (i < 100) {
        if (i == 10) {
            return i;
        }
        i = i + 1;
    }

    return -1;
}
EOF

# Test 07: Empty blocks
cat > "$CTRL_DIR/07_empty_blocks.c" << 'EOF'
// TEST: Empty blocks (SHOULD COMPILE)
int main() {
    int x;
    x = 5;

    if (x > 0) {
    }

    while (x < 10) {
        x = x + 1;
    }

    {
    }

    return x;
}
EOF

# Test 08: Multiple returns
cat > "$CTRL_DIR/08_multiple_returns.c" << 'EOF'
// TEST: Multiple return statements (SHOULD COMPILE)
int abs(int x) {
    if (x < 0) {
        return -x;
    } else {
        return x;
    }
}

int main() {
    return abs(-42);
}
EOF

# Test 09: Complex nested control
cat > "$CTRL_DIR/09_complex_nested.c" << 'EOF'
// TEST: Complex nested control structures (SHOULD COMPILE)
int main() {
    int i;
    int j;
    int result;

    i = 0;
    result = 0;

    while (i < 10) {
        if (i % 2 == 0) {
            j = 0;
            while (j < i) {
                if (j > 5) {
                    result = result + 2;
                } else {
                    result = result + 1;
                }
                j = j + 1;
            }
        } else {
            result = result + i;
        }
        i = i + 1;
    }

    return result;
}
EOF

# Test 10: Empty statements
cat > "$CTRL_DIR/10_empty_statements.c" << 'EOF'
// TEST: Empty statements (SHOULD COMPILE)
int main() {
    int x;
    x = 5;
    ;
    ;
    x = x + 1;
    ;
    return x;
}
EOF

#============================================================================
# ERROR TESTS - Syntax and semantic errors
#============================================================================
echo "Generating error tests..."

# Test 01: Missing semicolon
cat > "$ERROR_DIR/01_missing_semicolon.c" << 'EOF'
// TEST: Missing semicolon (SHOULD FAIL)
int main() {
    int x
    return 0;
}
EOF

# Test 02: Unmatched braces
cat > "$ERROR_DIR/02_unmatched_braces.c" << 'EOF'
// TEST: Unmatched braces (SHOULD FAIL)
int main() {
    int x;
    x = 5;
    return x;

EOF

# Test 03: Missing function body
cat > "$ERROR_DIR/03_missing_function_body.c" << 'EOF'
// TEST: Missing function body (SHOULD FAIL)
int foo();

int main() {
    return 0;
}
EOF

# Test 04: Invalid type
cat > "$ERROR_DIR/04_invalid_type.c" << 'EOF'
// TEST: Invalid type (SHOULD FAIL)
string x;  // ERROR: string not a valid type

int main() {
    return 0;
}
EOF

# Test 05: Void variable
cat > "$ERROR_DIR/05_void_variable.c" << 'EOF'
// TEST: Void variable (SHOULD FAIL)
int main() {
    void x;  // ERROR: cannot have void variable
    return 0;
}
EOF

# Test 06: Missing return type
cat > "$ERROR_DIR/06_missing_return_type.c" << 'EOF'
// TEST: Missing return type (SHOULD FAIL)
foo() {  // ERROR: no return type
    return 42;
}

int main() {
    return 0;
}
EOF

# Test 07: Type mismatch in assignment
cat > "$ERROR_DIR/07_type_mismatch.c" << 'EOF'
// TEST: Type mismatch (narrowing) (SHOULD FAIL)
int main() {
    int x;
    float f;

    f = 3.14;
    x = f;  // ERROR: narrowing

    return 0;
}
EOF

# Test 08: Undefined variable
cat > "$ERROR_DIR/08_undefined_variable.c" << 'EOF'
// TEST: Undefined variable (SHOULD FAIL)
int main() {
    x = 42;  // ERROR: x not defined
    return x;
}
EOF

# Test 09: Return value from void function
cat > "$ERROR_DIR/09_void_return_value.c" << 'EOF'
// TEST: Return value from void function (SHOULD FAIL)
void foo() {
    return 42;  // ERROR: void function cannot return value
}

int main() {
    foo();
    return 0;
}
EOF

# Test 10: Missing return in non-void function
cat > "$ERROR_DIR/10_missing_return.c" << 'EOF'
// TEST: Missing return in non-void function (SHOULD COMPILE - default return)
int foo() {
    int x;
    x = 42;
    // Missing return - compiler adds default
}

int main() {
    return foo();
}
EOF

echo "Test generation complete!"
echo ""
echo "Directory structure:"
echo "  type_tests/   - 10 type system tests"
echo "  scope_tests/  - 10 scope rule tests"
echo "  func_tests/   - 10 function semantic tests"
echo "  expr_tests/   - 10 expression/operator tests"
echo "  ctrl_tests/   - 10 control flow tests"
echo "  error_tests/  - 10 error detection tests"
echo ""
echo "Total: 60 comprehensive test cases"
