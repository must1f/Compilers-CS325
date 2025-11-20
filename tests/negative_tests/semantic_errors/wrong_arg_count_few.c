// INVALID - Type Error: wrong number of arguments
// Expected: "function expects X arguments, got Y"
int add(int a, int b) {
    return a + b;
}

int main() {
    int result;
    result = add(5);  // ERROR: missing second argument
    return result;
}
