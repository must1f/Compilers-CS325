// TEST: Narrowing in conditionals (SHOULD COMPILE)
int main() {
    int i;
    float f;
    i = 5;
    f = 3.14;

    if (i) {     // int→bool in conditional: OK
        i = 1;
    }

    if (f) {     // float→bool in conditional: OK
        i = 2;
    }

    return i;
}
