// INVALID - Type Error: int function needs return value
// Expected: "function must return a value"
int getNumber() {
    int x;
    x = 5;
    return;  // ERROR: int function with empty return
}
