// TEST: Short-circuit evaluation (SHOULD COMPILE)
int side_effect() {
    return 1;
}

int main() {
    int x;
    bool result;

    x = 0;
    result = x && side_effect();  // side_effect() not called

    x = 1;
    result = x || side_effect();  // side_effect() not called

    return 0;
}
