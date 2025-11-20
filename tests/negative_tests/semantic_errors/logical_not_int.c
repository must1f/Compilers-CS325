// INVALID - Type Error: logical NOT requires bool
int main() {
    int x;
    bool result;
    x = 5;
    result = !x;  // ERROR: ! requires bool operand (int to bool is narrowing)
    return 0;
}
