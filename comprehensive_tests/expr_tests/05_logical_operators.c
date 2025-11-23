// TEST: Logical operators (SHOULD COMPILE)
int main() {
    bool a;
    bool b;
    bool result;

    a = true;
    b = false;

    result = a && b;
    result = a || b;
    result = !a;
    result = !(a && b);
    result = (!a) || (!b);

    return 0;
}
