// INVALID - Scope Error: array not declared
// Expected: "undefined variable 'arr'"
int main() {
    int x;
    x = arr[0];  // ERROR: arr is not declared
    return x;
}
