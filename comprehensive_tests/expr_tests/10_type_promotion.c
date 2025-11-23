// TEST: Type promotion in binary ops (SHOULD COMPILE)
int main() {
    int i;
    float f;
    float result;

    i = 10;
    f = 3.14;

    result = i + f;   // i promoted to float
    result = f * i;   // i promoted to float
    result = i / f;   // i promoted to float

    return 0;
}
