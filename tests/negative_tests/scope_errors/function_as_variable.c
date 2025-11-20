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
