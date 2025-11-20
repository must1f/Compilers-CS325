// Tests if forward references work
int foo() {
    return bar();  // ERROR if bar not declared yet (no forward decl)
}

int bar() {
    return 42;
}
