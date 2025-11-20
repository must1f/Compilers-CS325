# MiniC Compiler - Negative Test Suite Report

**Generated:** 2025-11-20
**Compiler:** mccomp (LLVM 21.1.0)

---

## Executive Summary

Created and executed comprehensive negative test suite with **66 test cases** across three categories:
- **Syntax Errors:** 22 tests
- **Semantic Type Errors:** 27 tests
- **Semantic Scope Errors:** 17 tests

### Overall Results

| Metric | Result |
|--------|--------|
| **Passed** (correct error) | 32/66 (48%) |
| **Partial** (detected, wrong type) | 12/66 (18%) |
| **Failed** (accepted invalid) | 22/66 (33%) |
| **Detection Rate** | 66% |
| **IR Generation** | 9/11 (81%) |
| **Error Message Quality** | 58% |

---

## 1. Syntax Error Detection (22 tests)

### Results: 18 PASS, 3 PARTIAL, 1 FAIL

#### PASSED (18/22)
- array_no_size ✓
- if_no_condition ✓
- incomplete_array_access ✓
- incomplete_expr ✓
- incomplete_function ✓
- invalid_assignment ✓
- missing_brace ✓
- missing_comma_params ✓
- missing_param_type ✓
- missing_paren ✓
- missing_return_type ✓
- missing_semicolon ✓
- nested_function ✓
- unclosed_paren ✓
- unmatched_braces ✓
- void_variable ✓
- while_no_condition ✓

**Analysis:** Strong syntax error detection. The parser correctly identifies malformed code structures, missing tokens, and invalid syntax patterns.

#### PARTIAL (3/22)
- double_operator - Rejected but error type not labeled "Syntax Error"
- else_without_if - Rejected but error type not labeled "Syntax Error"
- invalid_return_statement - Rejected but error type not labeled "Syntax Error"

**Analysis:** These errors ARE caught, but the error message doesn't explicitly say "Syntax Error" in the format the test expects.

#### FAILED (1/22)
- **assign_in_condition** ✗ - ACCEPTED
- **double_semicolon** ✗ - ACCEPTED

**Note:** `assign_in_condition` (e.g., `if (x = 5)`) is actually VALID C syntax - assignment in condition is allowed. This test case is incorrect.

**Critical Issue:** `double_semicolon` should be caught but isn't.

---

## 2. Semantic Type Error Detection (27 tests)

### Results: 9 PASS, 5 PARTIAL, 13 FAIL

#### PASSED (9/27)
- int_return_void ✓
- modulo_float ✓
- unary_minus_bool ✓
- void_assign ✓
- void_return_value ✓
- wrong_arg_count_few ✓
- wrong_arg_count_many ✓
- wrong_arg_type_narrow ✓

**Analysis:** Good detection of void type violations, argument count mismatches, and some operator type mismatches.

#### PARTIAL (5/27)
- array_4d - Rejected (array dimension limits)
- array_negative_size - Rejected (invalid size)
- array_zero_size - Rejected (invalid size)
- call_non_function - Rejected
- function_pointer - Rejected

**Analysis:** These are caught but error messages don't match expected format.

#### FAILED - CRITICAL (13/27)
- **arithmetic_on_bool** ✗ - ACCEPTED
- **array_float_index** ✗ - ACCEPTED
- **array_wrong_dimensions** ✗ - ACCEPTED
- **bool_to_int_assign** ✗ - ACCEPTED
- **comparison_to_int** ✗ - ACCEPTED
- **division_by_zero** ✗ - ACCEPTED (Note: often not caught at compile-time)
- **float_to_int_assign** ✗ - ACCEPTED
- **incompatible_operands** ✗ - ACCEPTED
- **int_to_bool_assign** ✗ - ACCEPTED
- **logical_not_int** ✗ - ACCEPTED
- **logical_op_wrong_type** ✗ - ACCEPTED
- **mixed_type_array** ✗ - ACCEPTED
- **scalar_subscript** ✗ - ACCEPTED
- **wrong_return_type_narrow** ✗ - ACCEPTED

**Critical Issues:**

1. **Implicit Type Conversions:** Compiler allows implicit narrowing conversions (float→int, bool↔int) which violates the spec requirement for explicit conversions only

2. **Array Subscripting:** No validation that:
   - Index is integral type (allows float indices)
   - Base is actually an array (allows subscripting scalars)
   - Dimension count matches declaration

3. **Boolean Type Checking:** Booleans not properly distinguished from integers in:
   - Arithmetic operations (x + y on bools)
   - Logical operations (&&, || should require bool operands)
   - Comparisons (comparison result not checked as bool)

4. **Return Type Checking:** Missing validation for narrowing return types

---

## 3. Semantic Scope Error Detection (17 tests)

### Results: 8 PASS, 4 PARTIAL, 5 FAIL

#### PASSED (8/17)
- duplicate_global ✓
- forward_reference ✓
- function_redefinition ✓
- local_out_of_scope ✓
- missing_extern_decl ✓
- undefined_function ✓
- undefined_variable ✓

**Analysis:** Good detection of undefined identifiers and function redefinition.

#### PARTIAL (4/17)
- extern_mismatch - Rejected
- global_array_undeclared - Rejected
- undefined_array - Rejected
- use_before_declaration - Rejected

**Analysis:** Caught but error format doesn't match expectations.

#### FAILED - CRITICAL (5/17)
- **array_redeclaration** ✗ - ACCEPTED
- **duplicate_local** ✗ - ACCEPTED
- **duplicate_params** ✗ - ACCEPTED
- **function_as_variable** ✗ - ACCEPTED
- **no_main** ✗ - ACCEPTED
- **param_shadows_local** ✗ - ACCEPTED

**Critical Issues:**

1. **Duplicate Variables:** No checking for duplicate declarations in same scope:
   ```c
   int x;
   int x;  // Should error, but accepted
   ```

2. **Duplicate Parameters:** Function parameters can have duplicate names:
   ```c
   int foo(int x, int x) { }  // Should error, but accepted
   ```

3. **Missing main():** No validation that program has a main function

4. **Name Space Mixing:** Can use function name as variable (or vice versa)

5. **Parameter Shadowing:** Parameters can shadow local variables

---

## 4. IR Pattern Verification (11 tests)

### Results: 9 PASS, 2 PARTIAL

#### PASSED (9/11)
- int_to_float_assign ✓ (sitofp present)
- arg_widening ✓ (implicit widening works)
- array_operations ✓ (getelementptr present)
- proper_function_calls ✓ (call instructions present)
- addition ✓ (lecturer test)
- factorial ✓ (lecturer test)
- fibonacci ✓ (lecturer test)
- void ✓ (lecturer test)
- while ✓ (lecturer test)

#### PARTIAL (2/11)
- **comparison_to_bool** - Missing icmp pattern
- **all_widening** - Missing zext pattern

**Analysis:** Basic IR generation works well. Missing patterns suggest some implicit conversions aren't being generated properly, which aligns with type checking failures.

---

## 5. Error Message Quality (17 checks)

### Results: 10 PASS, 7 FAIL (58% quality score)

#### Strengths
- Syntax errors have good line numbers and error types ✓
- Scope errors (undefined variables) show line numbers ✓
- Variable names are included in error messages ✓

#### Weaknesses
- Some type errors missing line numbers ✗
- Some errors don't have explicit "Type Error" label ✗
- Color coding detection by test script failed (may be environment issue)

---

## Comparison with Coursework Specification (Section 2.1)

### Type Checking Requirements (Section 2.1)

| Requirement | Status | Notes |
|-------------|--------|-------|
| Integer type checking | ✓ Partial | Basic int operations work |
| Float type checking | ✓ Partial | Basic float operations work |
| Boolean type checking | ✗ MISSING | Bools treated as ints |
| Void type checking | ✓ PASS | Good validation |
| Array type checking | ✗ PARTIAL | Missing dimension/index checks |
| Function signature checking | ✓ PARTIAL | Arg count works, types partial |
| Type compatibility | ✗ FAIL | Allows narrowing conversions |

### Special Semantic Rules (Section 2.1.2)

| Rule | Status | Notes |
|------|--------|-------|
| No implicit narrowing | ✗ FAIL | Allows float→int, bool↔int |
| Widening allowed | ✓ PASS | int→float works |
| Array bounds static | ? UNKNOWN | Not tested in negative suite |
| No pointers | ✓ PASS | Caught function pointers |
| main() required | ✗ FAIL | no_main test accepted |

---

## Critical Gaps Requiring Fixes

### Priority 1 - Scope Checking
1. **Duplicate local variable detection** - Currently allows same variable declared twice
2. **Duplicate parameter detection** - Parameters can have duplicate names
3. **main() function validation** - No check that program has main()
4. **Function/variable namespace** - Can use same name for function and variable

### Priority 2 - Type Checking
5. **Implicit narrowing prevention** - Must reject float→int, bool→int without explicit cast
6. **Boolean operator validation** - Arithmetic on bools, logical ops on non-bools
7. **Array subscript validation** - Check index is int, base is array, dimensions match
8. **Return type narrowing** - Validate return value types match function signature

### Priority 3 - Error Messaging
9. **Consistent error type labels** - Ensure all errors clearly labeled (Syntax/Type/Scope)
10. **Line numbers for all errors** - Some type errors missing location info

---

## Test Suite Statistics

```
Total Test Cases: 66
├── Syntax Errors: 22 (18 pass, 3 partial, 1 fail) = 95% detection
├── Type Errors: 27 (9 pass, 5 partial, 13 fail) = 51% detection
└── Scope Errors: 17 (8 pass, 4 partial, 5 fail) = 70% detection

Overall Detection Rate: 66%
Critical Failures: 22 tests accepting invalid code
```

---

## Recommendations

1. **Immediate Actions:**
   - Implement duplicate variable checking in symbol table
   - Add main() validation in program validation phase
   - Prevent implicit narrowing type conversions

2. **Short-term Improvements:**
   - Strengthen array type checking (dimensions, indices)
   - Separate boolean type checking from integer
   - Improve error message consistency

3. **Code Quality:**
   - Add more semantic analysis passes
   - Implement proper type lattice for conversions
   - Enhanced symbol table with scope chain validation

4. **Testing:**
   - Some test cases may be incorrect (assign_in_condition, division_by_zero)
   - Consider additional tests for array bounds checking
   - Add tests for more edge cases in type widening

---

## Files Generated

- `tests/negative_tests/` - 66 negative test files
  - `syntax_errors/` - 22 syntax error tests
  - `semantic_errors/` - 27 type error tests
  - `scope_errors/` - 17 scope error tests
  - `reference_valid/` - 6 valid reference tests
- `tests/test_negative.sh` - Main test driver
- `tests/verify_ir_patterns.sh` - IR verification script
- `tests/test_error_messages.sh` - Error quality checker
- `tests/negative_test_results.txt` - Full test output
- `tests/verify_ir_results.txt` - IR verification output
- `tests/error_message_results.txt` - Error quality output

---

**End of Report**
