// TEST: Parentheses override precedence (SHOULD COMPILE)
int main() {
    int result;

    result = (2 + 3) * 4;   // Should be 20
    result = 2 + (3 * 4);   // Should be 14
    result = (10 - 6) / 2;  // Should be 2

    return result;
}
