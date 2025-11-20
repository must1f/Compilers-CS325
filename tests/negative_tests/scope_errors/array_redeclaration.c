// INVALID - Scope Error: array redeclared
// Expected: "redeclaration of 'arr'"
int main() {
    int arr[10];
    int arr[20];  // ERROR: redeclaration
    return arr[0];
}
