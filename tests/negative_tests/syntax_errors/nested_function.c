// INVALID - Syntax Error: functions cannot be nested
// Expected: "unexpected function declaration"
int outer() {
    int inner() {  // ERROR: nested functions not allowed
        return 1;
    }
    return inner();
}
