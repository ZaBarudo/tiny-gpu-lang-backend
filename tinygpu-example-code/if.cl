#define MAT_A_START 0    // First matrix starts at address 0
#define MAT_B_START 8   // Second matrix at 32 (8 elements x 4 bytes)
#define MAT_C_START 16   // Results at 64

__kernel void simple_matrix_ifelse() {
    int i = get_global_id(0);
    
    // Direct memory access (no pointers)
    int a = *((__global int*)(MAT_A_START + i));
    int b = *((__global int*)(MAT_B_START + i));
    
    // Simple if-else on matrix elements
    if (a > b) {
        *((__global int*)(MAT_C_START + i)) = a;  // Store A's value
    } else {
        *((__global int*)(MAT_C_START + i)) = b;  // Store B's value
    }
}