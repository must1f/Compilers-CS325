// INVALID - if function pointers aren't supported
int foo() {
    return 42;
}

int main() {
    int x;
    x = foo;  // ERROR: cannot assign function to variable (if not supported)
    return x;
}
