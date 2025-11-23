// TEST: Too many arguments (SHOULD FAIL)
int add(int a, int b) {
    return a + b;
}

int main() {
    return add(5, 10, 15);  // ERROR: expects 2 args, got 3
}
