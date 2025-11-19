extern int print_int(int X);

int test_3d() {
    int cube[2][3][4];
    int i;
    
    cube[0][1][2] = 42;
    cube[1][2][3] = 99;
    
    i = cube[0][1][2] + cube[1][2][3];
    
    return i;
}
