// INVALID - Scope Error: print_int not declared
// Expected: "undefined function 'print_int'"
int main() {
    print_int(42);  // ERROR: print_int not declared with extern
    return 0;
}
