// INVALID - Type Error: modulo requires integer operands
// Expected: "modulo operator requires integer types"
int main() {
    float x;
    float y;
    float z;
    x = 5.5;
    y = 2.5;
    z = x % y;  // ERROR: % requires int operands
    return 0;
}
