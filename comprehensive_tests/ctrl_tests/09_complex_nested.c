// TEST: Complex nested control structures (SHOULD COMPILE)
int main() {
    int i;
    int j;
    int result;

    i = 0;
    result = 0;

    while (i < 10) {
        if (i % 2 == 0) {
            j = 0;
            while (j < i) {
                if (j > 5) {
                    result = result + 2;
                } else {
                    result = result + 1;
                }
                j = j + 1;
            }
        } else {
            result = result + i;
        }
        i = i + 1;
    }

    return result;
}
