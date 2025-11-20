// INVALID - Type Error: return type narrowing
// Expected: "Type mismatch in return statement"
float getNumber() {
    int x;
    x = 5;
    return x;  // ERROR: returning int from float function (narrowing not allowed)
}
