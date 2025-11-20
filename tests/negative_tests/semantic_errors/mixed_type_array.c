// This tests if array elements have consistent types
int main() {
    int arr[5];
    arr[0] = 1;
    arr[1] = 2.5;  // ERROR: float to int narrowing in array element
    return arr[0];
}
