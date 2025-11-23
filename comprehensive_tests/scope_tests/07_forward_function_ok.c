// TEST: Forward function reference (SHOULD COMPILE)
int helper();

int main() {
    return helper();
}

int helper() {
    return 42;
}
