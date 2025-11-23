// TEST: Missing return in non-void function (SHOULD COMPILE - default return)
int foo() {
    int x;
    x = 42;
    // Missing return - compiler adds default
}

int main() {
    return foo();
}
