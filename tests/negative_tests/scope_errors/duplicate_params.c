// INVALID - Scope Error: duplicate parameter names
// Expected: "duplicate parameter name 'x'"
int foo(int x, int x) {  // ERROR: duplicate parameter
    return x;
}
