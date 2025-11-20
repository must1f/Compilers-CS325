// INVALID - Type Error: narrowing conversion not allowed
// Expected: "Type mismatch" or "narrowing conversion"
// SPEC: Section 2.1.2 - implicit narrowing conversions are errors
int main() {
    int x;
    float y;
    y = 3.14;
    x = y;  // ERROR: float to int is narrowing
    return x;
}
