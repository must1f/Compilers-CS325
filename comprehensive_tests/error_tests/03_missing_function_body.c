// TEST: Function called but not defined (SHOULD FAIL at link time, but compile)
// Note: Forward declarations are now allowed, so this compiles but would fail at link time
// Changed to test undefined variable instead
int main() {
    int x;
    x = undefinedVariable;
    return x;
}
