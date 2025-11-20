// INVALID - Syntax Error: liter cannot be lvalue
// Expected: "expected IDENT or array access"
int main() {
    5 = 10;  // ERROR: cannot assign to literal
    return 0;
}
