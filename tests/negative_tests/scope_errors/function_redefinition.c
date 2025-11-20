// INVALID - Scope Error: function redefined
// Expected: "redefinition of function 'foo'"
int foo(int x) {
    return x * 2;
}

int foo(int y) {  // ERROR: foo already defined
    return y * 3;
}
