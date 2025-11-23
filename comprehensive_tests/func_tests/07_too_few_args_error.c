// TEST: Too few arguments (SHOULD FAIL)
int add(int a, int b) {
    return a + b;
}

int main() {
    return add(5);  // ERROR: expects 2 args, got 1
}
