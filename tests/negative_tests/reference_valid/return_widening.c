// VALID - return type widening is allowed
// Should have: sitofp conversion in return
float getNumber() {
    int x;
    x = 5;
    return x;  // Actually this SHOULD be valid as it's widening!
}

int main() {
    float result;
    result = getNumber();
    return 0;
}
