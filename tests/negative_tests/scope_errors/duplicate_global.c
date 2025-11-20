// INVALID - Scope Error: duplicate global declaration
// Expected: "redeclaration of 'x'"
int x;
float x;  // ERROR: redeclaration of global x

int main() {
    return 0;
}
