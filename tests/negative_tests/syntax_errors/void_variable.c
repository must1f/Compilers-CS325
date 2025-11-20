// INVALID - Syntax Error: cannot declare void variable
// Expected: "variable cannot have void type"
int main() {
    void x;  // ERROR: void is not a valid variable type
    return 0;
}
