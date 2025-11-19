extern int print_int(int X);

int test_init() {
    int arr[5];
    int i;
    
    // Array should be uninitialized (LLVM allocates but doesn't zero)
    // We initialize it explicitly
    i = 0;
    while (i < 5) {
        arr[i] = i * 10;
        i = i + 1;
    }
    
    return arr[2];  // Should return 20
}
