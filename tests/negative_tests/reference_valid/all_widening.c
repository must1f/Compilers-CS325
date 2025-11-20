// VALID - demonstrates all allowed widening conversions
// IR should show: sitofp, zext, etc.
int main() {
    float f;
    int i;
    bool b;
    
    // int to float (widening - OK)
    i = 5;
    f = i;  // sitofp
    
    // bool to int in condition (OK)
    b = true;
    if (b) {
        i = 1;
    }
    
    return 0;
}
