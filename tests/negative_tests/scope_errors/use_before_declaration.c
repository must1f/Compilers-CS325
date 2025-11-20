// INVALID - Scope Error: use before declaration
// Expected: "undefined variable 'x'"
int main() {
    int result;
    result = x + 5;  // ERROR: x not yet declared
    int x;
    x = 10;
    return result;
}
