// TEST: Missing return type (SHOULD FAIL)
foo() {  // ERROR: no return type
    return 42;
}

int main() {
    return 0;
}
