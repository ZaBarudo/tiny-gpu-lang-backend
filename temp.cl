__kernel void vector_add(__global const int *A, 
                        __global const int *B, 
                        __global int *C, 
                        const int N) {
    int d = get_global_id(0);

    C[4] = A[4] + B[d];

}
