// INVALID - Syntax Error: incomplete array subscript
int main() {
    int arr[10];
    int x;
    x = arr[;  // ERROR: missing index expression
    return x;
}
