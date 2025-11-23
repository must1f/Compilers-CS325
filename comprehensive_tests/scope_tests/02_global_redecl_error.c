// TEST: Global redeclaration (SHOULD FAIL)
int global_var;
int global_var;  // ERROR: redeclaration

int main() {
    return 0;
}
