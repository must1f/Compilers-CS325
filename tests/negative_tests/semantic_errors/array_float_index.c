// INVALID - Type Error: array index must be integer
// Expected: "array index must be integer type"
int main() {
    int arr[10];
    float idx;
    int result;
    idx = 3.14;
    result = arr[idx];  // ERROR: float index
    return result;
}
