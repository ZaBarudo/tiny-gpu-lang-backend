__kernel void vector_add(__global const int* A,
                        __global const int* B,
                        __global int* C) {
    // Calculate global thread ID (equivalent to your assembly calculation)
    int i = get_global_id(0);
    
    // Load elements from global memory
    int a = A[i];
    int b = B[i];
    
    // Compute sum
    int c = a + b;
    
    // Store result
    C[i] = c;
}