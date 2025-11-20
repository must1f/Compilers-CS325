// INVALID - Type Error: int to bool is narrowing outside conditionals
// Expected: "Type mismatch"
int main() {
    bool x;
    int y;
    y = 5;
    x = y;  // ERROR: int to bool narrowing (not in condition context)
    return 0;
}
