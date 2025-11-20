// INVALID - Syntax Error: incomplete expression
// Expected: "expected expression"
int main() {
    int x;
    x = ;  // ERROR: missing expression after '='
    return x;
}
