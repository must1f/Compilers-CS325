// INVALID - Type Error: void function cannot return value
// Expected: "void function cannot return a value"
void foo() {
    return 5;  // ERROR: void function returning value
}
