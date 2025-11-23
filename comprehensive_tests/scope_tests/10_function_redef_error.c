// TEST: Function redefinition (SHOULD FAIL)
int foo() {
    return 1;
}

int foo() {  // ERROR: redefinition
    return 2;
}

int main() {
    return 0;
}
