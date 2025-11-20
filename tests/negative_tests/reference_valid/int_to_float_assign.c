// VALID - widening conversion (int to float) is allowed
// Should produce: %conv = sitofp i32 %y to float
int main() {
    float x;
    int y;
    y = 3;
    x = y;  // OK: int to float is widening (sitofp)
    return 0;
}
