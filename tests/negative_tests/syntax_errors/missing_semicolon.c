// INVALID - Syntax Error: missing semicolon
// Expected: "expected ';' to end expression statement"
int main() {
    int x;
    x = 5  // ERROR: missing semicolon
    return x;
}
