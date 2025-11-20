// INVALID - Semantic Error: max 3 dimensions
int main() {
    int arr[2][3][4][5];  // ERROR: 4D arrays not supported
    return 0;
}
