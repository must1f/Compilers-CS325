// INVALID - Syntax Error: missing comma between parameters
// Expected: "expected ',' or ')'"
int foo(int x int y) {  // ERROR: missing comma
    return x + y;
}
