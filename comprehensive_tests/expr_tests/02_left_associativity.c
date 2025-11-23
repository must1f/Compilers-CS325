// TEST: Left associativity (SHOULD COMPILE)
int main() {
    int result;
    result = 10 - 5 - 2;  // Should be 3 (left-to-right)
    result = 20 / 4 / 2;  // Should be 2 (left-to-right)
    return result;
}
