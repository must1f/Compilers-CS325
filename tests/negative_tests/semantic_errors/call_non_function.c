// INVALID - Type Error: variable is not callable
// Expected: "not a function"
int main() {
    int x;
    int result;
    x = 5;
    result = x();  // ERROR: x is not a function
    return result;
}
