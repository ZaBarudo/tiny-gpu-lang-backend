#define MATRIX_A_BASE 0
#define MATRIX_B_BASE 8
#define MATRIX_C_BASE 16

__kernel void vector_add() {
    int i = get_global_id(0);
    int a = *((__global int*)(MATRIX_A_BASE + i));
    int b = *((__global int*)(MATRIX_B_BASE + i));
    *((__global int*)(MATRIX_C_BASE + i)) = a + b;
}
