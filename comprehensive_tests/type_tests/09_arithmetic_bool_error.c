// TEST: Arithmetic operators require numeric types (SHOULD FAIL)
int main() {
    bool x;
    bool y;
    int result;

    x = true;
    y = false;
    result = x + y;  // Arithmetic on bool: ERROR

    return 0;
}
