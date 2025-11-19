// Test local array declaration
int main() {
    int localArr[5];
    float localMatrix[2][3];
    int i;

    i = 0;
    localArr[i] = 10;
    localMatrix[i][i + 1] = 3.14;

    return localArr[i];
}
