// INVALID - Type Error: cannot subscript non-array
// Expected: "subscripted value is not an array"
int main() {
    int x;
    int result;
    x = 5;
    result = x[0];  // ERROR: x is not an array
    return result;
}
