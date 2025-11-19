// Test invalid array syntax - should produce errors

// This should error: no size specified
// int arr[];

// This should error: variable size
// int x;
// int arr2[x];

// This should error: 4D array (more than 3 dimensions)
int arr4d[2][3][4][5];

int main() {
    return 0;
}
