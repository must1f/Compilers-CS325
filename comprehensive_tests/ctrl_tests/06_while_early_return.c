// TEST: While with early return (SHOULD COMPILE)
int main() {
    int i;

    i = 0;
    while (i < 100) {
        if (i == 10) {
            return i;
        }
        i = i + 1;
    }

    return -1;
}
