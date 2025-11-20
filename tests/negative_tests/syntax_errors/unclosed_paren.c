// INVALID - Syntax Error: unclosed parenthesis
// Expected: "expected ')'"
int main() {
    int x;
    x = (5 + 3;  // ERROR: missing ')'
    return x;
}
