// INVALID - Syntax Error: if requires condition
// Expected: "expected '(' after if"
int main() {
    int x;
    if {  // ERROR: missing condition
        x = 5;
    }
    return x;
}
