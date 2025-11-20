// INVALID - Scope Error: extern declaration doesn't match definition
// Expected: "conflicting types"
extern int foo(int x);

float foo(int x) {  // ERROR: return type mismatch with extern
    return 3.14;
}
