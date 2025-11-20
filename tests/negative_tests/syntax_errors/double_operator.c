// INVALID - Syntax Error: consecutive operators
// Expected: "expected expression" or "unexpected token"
int main() {
    int x;
    x = 5 ++ 3;  // ERROR: ++ is not a valid operator in MiniC
    return x;
}
