// INVALID - Type Error: argument narrowing not allowed
// Expected: "Type mismatch in function call"
int foo(int x) {
    return x * 2;
}

int main() {
    float y;
    int result;
    y = 3.14;
    result = foo(y);  // ERROR: passing float to int parameter (narrowing)
    return result;
}
