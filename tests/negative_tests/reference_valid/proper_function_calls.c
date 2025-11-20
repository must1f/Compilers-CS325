// VALID - correct function call semantics
extern int print_int(int x);

int add(int a, int b) {
    return a + b;
}

float multiply(float a, float b) {
    return a * b;
}

int main() {
    int x;
    int y;
    float f;
    
    x = 5;
    y = 10;
    x = add(x, y);  // correct types
    print_int(x);
    
    f = multiply(3.14, 2.0);  // correct float arguments
    
    return x;
}
