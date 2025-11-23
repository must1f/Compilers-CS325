// TEST: Nested while loops (SHOULD COMPILE)
int main() {
    int i;
    int j;
    int sum;

    i = 0;
    sum = 0;

    while (i < 5) {
        j = 0;
        while (j < 5) {
            sum = sum + 1;
            j = j + 1;
        }
        i = i + 1;
    }

    return sum;
}
