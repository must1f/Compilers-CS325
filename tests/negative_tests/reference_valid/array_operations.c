// VALID - proper array usage
// IR should show GEP instructions with proper indexing
int main() {
    int arr[10];
    int i;
    int sum;
    
    // Initialize array
    i = 0;
    while (i < 10) {
        arr[i] = i * 2;
        i = i + 1;
    }
    
    // Sum array
    sum = 0;
    i = 0;
    while (i < 10) {
        sum = sum + arr[i];
        i = i + 1;
    }
    
    return sum;
}
