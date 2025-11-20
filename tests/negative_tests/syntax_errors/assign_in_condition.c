// INVALID - Syntax Error: assignment vs equality
int main() {
    int x;
    if (x = 5) {  // This is assignment, not comparison - should work but unusual
        return x;
    }
    return 0;
}
