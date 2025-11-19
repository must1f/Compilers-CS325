// Test 2D array declaration and access
float matrix[3][4];

int main() {
    int i;
    int j;

    i = 0;
    j = 1;
    matrix[i][j] = matrix[j][i] + 1.0;

    return 0;
}
