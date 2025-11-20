// This might pass compilation but is semantically questionable
int main() {
    int x;
    x = 5 / 0;  // WARNING: division by zero
    return x;
}
