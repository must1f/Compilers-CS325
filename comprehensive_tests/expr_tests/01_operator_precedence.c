// TEST: Operator precedence (SHOULD COMPILE)
int main() {
    int result;
    result = 2 + 3 * 4;  // Should be 14, not 20
    result = 10 - 6 / 2;  // Should be 7, not 2
    return result;
}
