// INVALID - Type Error: too many arguments
// Expected: "function expects X arguments, got Y"
int add(int a, int b) {
    return a + b;
}

int main() {
    int result;
    result = add(5, 10, 15);  // ERROR: too many arguments
    return result;
}
