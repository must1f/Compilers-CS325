// INVALID - Syntax Error: parameter must have type
// Expected: "expected type specifier"
int foo(x) {  // ERROR: parameter 'x' has no type
    return x;
}
