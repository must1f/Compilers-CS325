// TEST: Return value from void function (SHOULD FAIL)
void foo() {
    return 42;  // ERROR: void function cannot return value
}

int main() {
    foo();
    return 0;
}
