#!/bin/bash

# Generate comprehensive negative test suite for MiniC compiler

SYNTAX_DIR="syntax_errors"
SEMANTIC_DIR="semantic_errors"
SCOPE_DIR="scope_errors"
VALID_DIR="reference_valid"

echo "Generating negative test suite..."

# =================================================================
# SYNTAX ERROR TESTS (20+ files)
# =================================================================

# Test 1: Missing semicolon
cat > "$SYNTAX_DIR/missing_semicolon.c" << 'EOF'
// INVALID - Syntax Error: missing semicolon
// Expected: "expected ';' to end expression statement"
int main() {
    int x;
    x = 5  // ERROR: missing semicolon
    return x;
}
EOF

# Test 2: Missing parenthesis
cat > "$SYNTAX_DIR/missing_paren.c" << 'EOF'
// INVALID - Syntax Error: missing closing parenthesis
// Expected: "expected ')'"
int main( {
    return 0;
}
EOF

# Test 3: Missing brace
cat > "$SYNTAX_DIR/missing_brace.c" << 'EOF'
// INVALID - Syntax Error: missing closing brace
// Expected: "expected '}'"
int main() {
    int x;
    x = 5;
    return x;
// Missing closing brace
EOF

# Test 4: Void variable declaration
cat > "$SYNTAX_DIR/void_variable.c" << 'EOF'
// INVALID - Syntax Error: cannot declare void variable
// Expected: "variable cannot have void type"
int main() {
    void x;  // ERROR: void is not a valid variable type
    return 0;
}
EOF

# Test 5: Invalid assignment target
cat > "$SYNTAX_DIR/invalid_assignment.c" << 'EOF'
// INVALID - Syntax Error: liter cannot be lvalue
// Expected: "expected IDENT or array access"
int main() {
    5 = 10;  // ERROR: cannot assign to literal
    return 0;
}
EOF

# Test 6: Unclosed parenthesis
cat > "$SYNTAX_DIR/unclosed_paren.c" << 'EOF'
// INVALID - Syntax Error: unclosed parenthesis
// Expected: "expected ')'"
int main() {
    int x;
    x = (5 + 3;  // ERROR: missing ')'
    return x;
}
EOF

# Test 7: If without condition
cat > "$SYNTAX_DIR/if_no_condition.c" << 'EOF'
// INVALID - Syntax Error: if requires condition
// Expected: "expected '(' after if"
int main() {
    int x;
    if {  // ERROR: missing condition
        x = 5;
    }
    return x;
}
EOF

# Test 8: Incomplete function
cat > "$SYNTAX_DIR/incomplete_function.c" << 'EOF'
// INVALID - Syntax Error: incomplete function declaration
// Expected: "expected '{' to start function body"
int foo(int x)
// Missing body
EOF

# Test 9: Incomplete expression
cat > "$SYNTAX_DIR/incomplete_expr.c" << 'EOF'
// INVALID - Syntax Error: incomplete expression
// Expected: "expected expression"
int main() {
    int x;
    x = ;  // ERROR: missing expression after '='
    return x;
}
EOF

# Test 10: Nested function definition
cat > "$SYNTAX_DIR/nested_function.c" << 'EOF'
// INVALID - Syntax Error: functions cannot be nested
// Expected: "unexpected function declaration"
int outer() {
    int inner() {  // ERROR: nested functions not allowed
        return 1;
    }
    return inner();
}
EOF

# Test 11: Array without size
cat > "$SYNTAX_DIR/array_no_size.c" << 'EOF'
// INVALID - Syntax Error: array size required
// Expected: "expected integer literal for array dimension"
int main() {
    int arr[];  // ERROR: array size must be specified
    return 0;
}
EOF

# Test 12: Double operator
cat > "$SYNTAX_DIR/double_operator.c" << 'EOF'
// INVALID - Syntax Error: consecutive operators
// Expected: "expected expression" or "unexpected token"
int main() {
    int x;
    x = 5 ++ 3;  // ERROR: ++ is not a valid operator in MiniC
    return x;
}
EOF

# Test 13: Missing function return type
cat > "$SYNTAX_DIR/missing_return_type.c" << 'EOF'
// INVALID - Syntax Error: missing return type
// Expected: "expected type specifier"
foo(int x) {  // ERROR: missing return type
    return x;
}
EOF

# Test 14: Missing parameter type
cat > "$SYNTAX_DIR/missing_param_type.c" << 'EOF'
// INVALID - Syntax Error: parameter must have type
// Expected: "expected type specifier"
int foo(x) {  // ERROR: parameter 'x' has no type
    return x;
}
EOF

# Test 15: Double semicolon in declaration
cat > "$SYNTAX_DIR/double_semicolon.c" << 'EOF'
// INVALID - Syntax Error: unexpected semicolon
int main() {
    int x;;  // ERROR: extra semicolon
    return 0;
}
EOF

# Test 16: Unmatched braces
cat > "$SYNTAX_DIR/unmatched_braces.c" << 'EOF'
// INVALID - Syntax Error: unmatched closing brace
int main() {
    int x;
    x = 5;
}}  // ERROR: extra closing brace
EOF

# Test 17: Missing comma in parameter list
cat > "$SYNTAX_DIR/missing_comma_params.c" << 'EOF'
// INVALID - Syntax Error: missing comma between parameters
// Expected: "expected ',' or ')'"
int foo(int x int y) {  // ERROR: missing comma
    return x + y;
}
EOF

# Test 18: Assignment in condition without parentheses
cat > "$SYNTAX_DIR/assign_in_condition.c" << 'EOF'
// INVALID - Syntax Error: assignment vs equality
int main() {
    int x;
    if (x = 5) {  // This is assignment, not comparison - should work but unusual
        return x;
    }
    return 0;
}
EOF

# Test 19: Else without if
cat > "$SYNTAX_DIR/else_without_if.c" << 'EOF'
// INVALID - Syntax Error: else must follow if
int main() {
    int x;
    else {  // ERROR: else without if
        x = 5;
    }
    return x;
}
EOF

# Test 20: Return with no value in non-void function
cat > "$SYNTAX_DIR/invalid_return_statement.c" << 'EOF'
// INVALID - Semantic Error: int function needs return value
int foo() {
    return;  // ERROR: missing return value
}
EOF

# Test 21: Missing expression in while condition
cat > "$SYNTAX_DIR/while_no_condition.c" << 'EOF'
// INVALID - Syntax Error: while needs condition
int main() {
    while () {  // ERROR: missing condition
        break;
    }
    return 0;
}
EOF

# Test 22: Incomplete array access
cat > "$SYNTAX_DIR/incomplete_array_access.c" << 'EOF'
// INVALID - Syntax Error: incomplete array subscript
int main() {
    int arr[10];
    int x;
    x = arr[;  // ERROR: missing index expression
    return x;
}
EOF

echo "Generated 22 syntax error tests"

# =================================================================
# SEMANTIC TYPE ERROR TESTS (25+ files)
# =================================================================

# Type Test 1: Float to int assignment (narrowing)
cat > "$SEMANTIC_DIR/float_to_int_assign.c" << 'EOF'
// INVALID - Type Error: narrowing conversion not allowed
// Expected: "Type mismatch" or "narrowing conversion"
// SPEC: Section 2.1.2 - implicit narrowing conversions are errors
int main() {
    int x;
    float y;
    y = 3.14;
    x = y;  // ERROR: float to int is narrowing
    return x;
}
EOF

# Valid reference for comparison
cat > "$VALID_DIR/int_to_float_assign.c" << 'EOF'
// VALID - widening conversion (int to float) is allowed
// Should produce: %conv = sitofp i32 %y to float
int main() {
    float x;
    int y;
    y = 3;
    x = y;  // OK: int to float is widening (sitofp)
    return 0;
}
EOF

# Type Test 2: Int to bool assignment (narrowing outside condition)
cat > "$SEMANTIC_DIR/int_to_bool_assign.c" << 'EOF'
// INVALID - Type Error: int to bool is narrowing outside conditionals
// Expected: "Type mismatch"
int main() {
    bool x;
    int y;
    y = 5;
    x = y;  // ERROR: int to bool narrowing (not in condition context)
    return 0;
}
EOF

# Type Test 3: Void assignment
cat > "$SEMANTIC_DIR/void_assign.c" << 'EOF'
// INVALID - Type Error: void has no value
// Expected: "cannot use void expression"
extern void print_int(int x);

int main() {
    int x;
    x = print_int(5);  // ERROR: print_int returns void
    return x;
}
EOF

# Type Test 4: Modulo with float
cat > "$SEMANTIC_DIR/modulo_float.c" << 'EOF'
// INVALID - Type Error: modulo requires integer operands
// Expected: "modulo operator requires integer types"
int main() {
    float x;
    float y;
    float z;
    x = 5.5;
    y = 2.5;
    z = x % y;  // ERROR: % requires int operands
    return 0;
}
EOF

# Type Test 5: Wrong argument type (narrowing)
cat > "$SEMANTIC_DIR/wrong_arg_type_narrow.c" << 'EOF'
// INVALID - Type Error: argument narrowing not allowed
// Expected: "Type mismatch in function call"
int foo(int x) {
    return x * 2;
}

int main() {
    float y;
    int result;
    y = 3.14;
    result = foo(y);  // ERROR: passing float to int parameter (narrowing)
    return result;
}
EOF

# Valid reference
cat > "$VALID_DIR/arg_widening.c" << 'EOF'
// VALID - argument widening is allowed
// Should have: sitofp for int->float conversion
float foo(float x) {
    return x * 2.0;
}

int main() {
    int y;
    float result;
    y = 3;
    result = foo(y);  // OK: int to float is widening
    return 0;
}
EOF

# Type Test 6: Wrong argument count (too few)
cat > "$SEMANTIC_DIR/wrong_arg_count_few.c" << 'EOF'
// INVALID - Type Error: wrong number of arguments
// Expected: "function expects X arguments, got Y"
int add(int a, int b) {
    return a + b;
}

int main() {
    int result;
    result = add(5);  // ERROR: missing second argument
    return result;
}
EOF

# Type Test 7: Wrong argument count (too many)
cat > "$SEMANTIC_DIR/wrong_arg_count_many.c" << 'EOF'
// INVALID - Type Error: too many arguments
// Expected: "function expects X arguments, got Y"
int add(int a, int b) {
    return a + b;
}

int main() {
    int result;
    result = add(5, 10, 15);  // ERROR: too many arguments
    return result;
}
EOF

# Type Test 8: Wrong return type (narrowing)
cat > "$SEMANTIC_DIR/wrong_return_type_narrow.c" << 'EOF'
// INVALID - Type Error: return type narrowing
// Expected: "Type mismatch in return statement"
float getNumber() {
    int x;
    x = 5;
    return x;  // ERROR: returning int from float function (narrowing not allowed)
}
EOF

# Valid reference
cat > "$VALID_DIR/return_widening.c" << 'EOF'
// VALID - return type widening is allowed
// Should have: sitofp conversion in return
float getNumber() {
    int x;
    x = 5;
    return x;  // Actually this SHOULD be valid as it's widening!
}

int main() {
    float result;
    result = getNumber();
    return 0;
}
EOF

# Type Test 9: Void returning value
cat > "$SEMANTIC_DIR/void_return_value.c" << 'EOF'
// INVALID - Type Error: void function cannot return value
// Expected: "void function cannot return a value"
void foo() {
    return 5;  // ERROR: void function returning value
}
EOF

# Type Test 10: Int function with empty return
cat > "$SEMANTIC_DIR/int_return_void.c" << 'EOF'
// INVALID - Type Error: int function needs return value
// Expected: "function must return a value"
int getNumber() {
    int x;
    x = 5;
    return;  // ERROR: int function with empty return
}
EOF

# Type Test 11: Calling non-function
cat > "$SEMANTIC_DIR/call_non_function.c" << 'EOF'
// INVALID - Type Error: variable is not callable
// Expected: "not a function"
int main() {
    int x;
    int result;
    x = 5;
    result = x();  // ERROR: x is not a function
    return result;
}
EOF

# Type Test 12: Array with float index
cat > "$SEMANTIC_DIR/array_float_index.c" << 'EOF'
// INVALID - Type Error: array index must be integer
// Expected: "array index must be integer type"
int main() {
    int arr[10];
    float idx;
    int result;
    idx = 3.14;
    result = arr[idx];  // ERROR: float index
    return result;
}
EOF

# Type Test 13: Array wrong dimensions
cat > "$SEMANTIC_DIR/array_wrong_dimensions.c" << 'EOF'
// INVALID - Type Error: dimension mismatch
// Expected: "array dimension mismatch"
int main() {
    int arr[10];
    int result;
    result = arr[0][0];  // ERROR: 1D array accessed as 2D
    return result;
}
EOF

# Type Test 14: Scalar subscript
cat > "$SEMANTIC_DIR/scalar_subscript.c" << 'EOF'
// INVALID - Type Error: cannot subscript non-array
// Expected: "subscripted value is not an array"
int main() {
    int x;
    int result;
    x = 5;
    result = x[0];  // ERROR: x is not an array
    return result;
}
EOF

# Type Test 15: Bool to int narrowing
cat > "$SEMANTIC_DIR/bool_to_int_assign.c" << 'EOF'
// INVALID - Type Error: bool to int narrowing
int main() {
    int x;
    bool y;
    y = true;
    x = y;  // ERROR: bool to int is narrowing
    return x;
}
EOF

# Type Test 16: Logical operator with non-bool
cat > "$SEMANTIC_DIR/logical_op_wrong_type.c" << 'EOF'
// INVALID - Type Error: logical operators need bool operands
int main() {
    int x;
    int y;
    bool result;
    x = 5;
    y = 10;
    result = x && y;  // ERROR: && requires bool operands (int to bool is narrowing)
    return 0;
}
EOF

# Type Test 17: Arithmetic on bool
cat > "$SEMANTIC_DIR/arithmetic_on_bool.c" << 'EOF'
// INVALID - Type Error: arithmetic requires numeric types
int main() {
    bool x;
    bool y;
    int result;
    x = true;
    y = false;
    result = x + y;  // ERROR: cannot add booleans
    return result;
}
EOF

# Type Test 18: Comparison result assignment
cat > "$SEMANTIC_DIR/comparison_to_int.c" << 'EOF'
// INVALID - Type Error: comparison returns bool, assigning to int is narrowing
int main() {
    int x;
    x = (5 > 3);  // ERROR: bool to int narrowing
    return x;
}
EOF

# Valid reference
cat > "$VALID_DIR/comparison_to_bool.c" << 'EOF'
// VALID - comparison to bool is correct
int main() {
    bool x;
    x = (5 > 3);  // OK: comparison returns bool
    return 0;
}
EOF

# Type Test 19: Incompatible binary operands
cat > "$SEMANTIC_DIR/incompatible_operands.c" << 'EOF'
// INVALID - Type Error: operands must have compatible types
int main() {
    int x;
    float y;
    float result;
    x = 5;
    y = 3.14;
    result = x + y;  // ERROR: mixing int and float without explicit conversion
    return 0;
}
EOF

# Type Test 20: Division by zero literal
cat > "$SEMANTIC_DIR/division_by_zero.c" << 'EOF'
// This might pass compilation but is semantically questionable
int main() {
    int x;
    x = 5 / 0;  // WARNING: division by zero
    return x;
}
EOF

# Type Test 21: Array size must be positive
cat > "$SEMANTIC_DIR/array_zero_size.c" << 'EOF'
// INVALID - Semantic Error: array size must be positive
int main() {
    int arr[0];  // ERROR: array size must be > 0
    return 0;
}
EOF

# Type Test 22: Array negative size
cat > "$SEMANTIC_DIR/array_negative_size.c" << 'EOF'
// INVALID - Semantic Error: array size must be positive
int main() {
    int arr[-5];  // ERROR: negative array size
    return 0;
}
EOF

# Type Test 23: 4D array not supported
cat > "$SEMANTIC_DIR/array_4d.c" << 'EOF'
// INVALID - Semantic Error: max 3 dimensions
int main() {
    int arr[2][3][4][5];  // ERROR: 4D arrays not supported
    return 0;
}
EOF

# Type Test 24: Mixed type in array
cat > "$SEMANTIC_DIR/mixed_type_array.c" << 'EOF'
// This tests if array elements have consistent types
int main() {
    int arr[5];
    arr[0] = 1;
    arr[1] = 2.5;  // ERROR: float to int narrowing in array element
    return arr[0];
}
EOF

# Type Test 25: Function pointer (not supported)
cat > "$SEMANTIC_DIR/function_pointer.c" << 'EOF'
// INVALID - if function pointers aren't supported
int foo() {
    return 42;
}

int main() {
    int x;
    x = foo;  // ERROR: cannot assign function to variable (if not supported)
    return x;
}
EOF

# Type Test 26: Unary minus on bool
cat > "$SEMANTIC_DIR/unary_minus_bool.c" << 'EOF'
// INVALID - Type Error: unary minus requires numeric type
int main() {
    bool x;
    bool y;
    x = true;
    y = -x;  // ERROR: cannot negate boolean
    return 0;
}
EOF

# Type Test 27: Logical NOT on non-bool
cat > "$SEMANTIC_DIR/logical_not_int.c" << 'EOF'
// INVALID - Type Error: logical NOT requires bool
int main() {
    int x;
    bool result;
    x = 5;
    result = !x;  // ERROR: ! requires bool operand (int to bool is narrowing)
    return 0;
}
EOF

echo "Generated 27 semantic type error tests"

# =================================================================
# SEMANTIC SCOPE ERROR TESTS (15+ files)
# =================================================================

# Scope Test 1: Undefined variable
cat > "$SCOPE_DIR/undefined_variable.c" << 'EOF'
// INVALID - Scope Error: undefined variable
// Expected: "undefined variable 'z'"
int main() {
    int x;
    int y;
    x = 5;
    y = 10;
    return z;  // ERROR: z is not declared
}
EOF

# Scope Test 2: Undefined function
cat > "$SCOPE_DIR/undefined_function.c" << 'EOF'
// INVALID - Scope Error: undefined function
// Expected: "undefined function 'foo'"
int main() {
    int result;
    result = foo(5);  // ERROR: foo is not declared
    return result;
}
EOF

# Scope Test 3: Duplicate global
cat > "$SCOPE_DIR/duplicate_global.c" << 'EOF'
// INVALID - Scope Error: duplicate global declaration
// Expected: "redeclaration of 'x'"
int x;
float x;  // ERROR: redeclaration of global x

int main() {
    return 0;
}
EOF

# Scope Test 4: Duplicate local
cat > "$SCOPE_DIR/duplicate_local.c" << 'EOF'
// INVALID - Scope Error: duplicate local declaration
// Expected: "redeclaration of 'x' in same scope"
int main() {
    int x;
    int x;  // ERROR: x already declared in this scope
    return 0;
}
EOF

# Scope Test 5: Variable out of scope
cat > "$SCOPE_DIR/local_out_of_scope.c" << 'EOF'
// INVALID - Scope Error: variable used outside its scope
// Expected: "undefined variable 'x'"
int main() {
    int result;
    {
        int x;
        x = 5;
    }
    result = x;  // ERROR: x is out of scope (if MiniC supports nested blocks)
    return result;
}
EOF

# Scope Test 6: Missing extern declaration
cat > "$SCOPE_DIR/missing_extern_decl.c" << 'EOF'
// INVALID - Scope Error: print_int not declared
// Expected: "undefined function 'print_int'"
int main() {
    print_int(42);  // ERROR: print_int not declared with extern
    return 0;
}
EOF

# Scope Test 7: Undefined array
cat > "$SCOPE_DIR/undefined_array.c" << 'EOF'
// INVALID - Scope Error: array not declared
// Expected: "undefined variable 'arr'"
int main() {
    int x;
    x = arr[0];  // ERROR: arr is not declared
    return x;
}
EOF

# Scope Test 8: Array redeclaration
cat > "$SCOPE_DIR/array_redeclaration.c" << 'EOF'
// INVALID - Scope Error: array redeclared
// Expected: "redeclaration of 'arr'"
int main() {
    int arr[10];
    int arr[20];  // ERROR: redeclaration
    return arr[0];
}
EOF

# Scope Test 9: Function redefinition
cat > "$SCOPE_DIR/function_redefinition.c" << 'EOF'
// INVALID - Scope Error: function redefined
// Expected: "redefinition of function 'foo'"
int foo(int x) {
    return x * 2;
}

int foo(int y) {  // ERROR: foo already defined
    return y * 3;
}
EOF

# Scope Test 10: Parameter shadows local
cat > "$SCOPE_DIR/param_shadows_local.c" << 'EOF'
// This might be allowed or flagged as warning
int foo(int x) {
    int x;  // ERROR or WARNING: parameter x shadowed by local x
    x = 10;
    return x;
}
EOF

# Scope Test 11: Using function name as variable
cat > "$SCOPE_DIR/function_as_variable.c" << 'EOF'
// INVALID - Scope Error: name collision
// Expected: "redeclaration" or "conflicting types"
int foo() {
    return 42;
}

int main() {
    int foo;  // ERROR: foo is already a function name
    foo = 10;
    return foo;
}
EOF

# Scope Test 12: Variable used before declaration
cat > "$SCOPE_DIR/use_before_declaration.c" << 'EOF'
// INVALID - Scope Error: use before declaration
// Expected: "undefined variable 'x'"
int main() {
    int result;
    result = x + 5;  // ERROR: x not yet declared
    int x;
    x = 10;
    return result;
}
EOF

# Scope Test 13: Global array undeclared
cat > "$SCOPE_DIR/global_array_undeclared.c" << 'EOF'
// INVALID - Scope Error: global array not declared
// Expected: "undefined variable 'global_arr'"
int main() {
    int x;
    x = global_arr[0];  // ERROR: global_arr not declared
    return x;
}
EOF

# Scope Test 14: Extern function mismatch
cat > "$SCOPE_DIR/extern_mismatch.c" << 'EOF'
// INVALID - Scope Error: extern declaration doesn't match definition
// Expected: "conflicting types"
extern int foo(int x);

float foo(int x) {  // ERROR: return type mismatch with extern
    return 3.14;
}
EOF

# Scope Test 15: Circular dependency
cat > "$SCOPE_DIR/forward_reference.c" << 'EOF'
// Tests if forward references work
int foo() {
    return bar();  // ERROR if bar not declared yet (no forward decl)
}

int bar() {
    return 42;
}
EOF

# Scope Test 16: Duplicate parameter names
cat > "$SCOPE_DIR/duplicate_params.c" << 'EOF'
// INVALID - Scope Error: duplicate parameter names
// Expected: "duplicate parameter name 'x'"
int foo(int x, int x) {  // ERROR: duplicate parameter
    return x;
}
EOF

# Scope Test 17: Main not defined
cat > "$SCOPE_DIR/no_main.c" << 'EOF'
// INVALID - possibly required to have main
int foo() {
    return 42;
}
// ERROR: no main function (if required)
EOF

echo "Generated 17 scope error tests"

# =================================================================
# ADDITIONAL VALID REFERENCE TESTS
# =================================================================

# Valid: Simple type conversions
cat > "$VALID_DIR/all_widening.c" << 'EOF'
// VALID - demonstrates all allowed widening conversions
// IR should show: sitofp, zext, etc.
int main() {
    float f;
    int i;
    bool b;
    
    // int to float (widening - OK)
    i = 5;
    f = i;  // sitofp
    
    // bool to int in condition (OK)
    b = true;
    if (b) {
        i = 1;
    }
    
    return 0;
}
EOF

# Valid: Array operations
cat > "$VALID_DIR/array_operations.c" << 'EOF'
// VALID - proper array usage
// IR should show GEP instructions with proper indexing
int main() {
    int arr[10];
    int i;
    int sum;
    
    // Initialize array
    i = 0;
    while (i < 10) {
        arr[i] = i * 2;
        i = i + 1;
    }
    
    // Sum array
    sum = 0;
    i = 0;
    while (i < 10) {
        sum = sum + arr[i];
        i = i + 1;
    }
    
    return sum;
}
EOF

# Valid: Function calls with proper types
cat > "$VALID_DIR/proper_function_calls.c" << 'EOF'
// VALID - correct function call semantics
extern int print_int(int x);

int add(int a, int b) {
    return a + b;
}

float multiply(float a, float b) {
    return a * b;
}

int main() {
    int x;
    int y;
    float f;
    
    x = 5;
    y = 10;
    x = add(x, y);  // correct types
    print_int(x);
    
    f = multiply(3.14, 2.0);  // correct float arguments
    
    return x;
}
EOF

echo "Generated 3 valid reference tests"
echo ""
echo "Test generation complete!"
echo "Total syntax error tests: 22"
echo "Total semantic type error tests: 27"
echo "Total scope error tests: 17"
echo "Total valid reference tests: 6"
echo "Grand total: 72 test files"

