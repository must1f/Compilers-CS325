// INVALID - Syntax Error: array size required
// Expected: "expected integer literal for array dimension"
int main() {
    int arr[];  // ERROR: array size must be specified
    return 0;
}
