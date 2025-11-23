// TEST: foo() with arguments (SHOULD FAIL)
int no_args() {
    return 42;
}

int main() {
    return no_args(5);  // ERROR: expects 0 args, got 1
}
