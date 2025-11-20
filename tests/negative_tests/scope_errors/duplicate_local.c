// INVALID - Scope Error: duplicate local declaration
// Expected: "redeclaration of 'x' in same scope"
int main() {
    int x;
    int x;  // ERROR: x already declared in this scope
    return 0;
}
