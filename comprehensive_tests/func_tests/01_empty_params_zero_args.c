// TEST: foo() means exactly zero arguments (SHOULD COMPILE)
int no_args() {
    return 42;
}

int main() {
    return no_args();  // OK: zero arguments
}
