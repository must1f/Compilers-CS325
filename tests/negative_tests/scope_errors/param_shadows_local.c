// This might be allowed or flagged as warning
int foo(int x) {
    int x;  // ERROR or WARNING: parameter x shadowed by local x
    x = 10;
    return x;
}
