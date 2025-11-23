// TEST: Multiple return statements (SHOULD COMPILE)
int abs(int x) {
    if (x < 0) {
        return -x;
    } else {
        return x;
    }
}

int main() {
    return abs(-42);
}
