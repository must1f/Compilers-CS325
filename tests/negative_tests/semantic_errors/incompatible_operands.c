// INVALID - Type Error: operands must have compatible types
int main() {
    int x;
    float y;
    float result;
    x = 5;
    y = 3.14;
    result = x + y;  // ERROR: mixing int and float without explicit conversion
    return 0;
}
