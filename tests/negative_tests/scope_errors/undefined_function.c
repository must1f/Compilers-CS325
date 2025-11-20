// INVALID - Scope Error: undefined function
// Expected: "undefined function 'foo'"
int main() {
    int result;
    result = foo(5);  // ERROR: foo is not declared
    return result;
}
