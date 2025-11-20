// INVALID - Type Error: dimension mismatch
// Expected: "array dimension mismatch"
int main() {
    int arr[10];
    int result;
    result = arr[0][0];  // ERROR: 1D array accessed as 2D
    return result;
}
