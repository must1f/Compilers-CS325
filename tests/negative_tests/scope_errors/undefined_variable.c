// INVALID - Scope Error: undefined variable
// Expected: "undefined variable 'z'"
int main() {
    int x;
    int y;
    x = 5;
    y = 10;
    return z;  // ERROR: z is not declared
}
