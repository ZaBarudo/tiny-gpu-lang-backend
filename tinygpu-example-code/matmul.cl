// OpenCL Kernel for Vector Addition
__kernel void add_vectors(__global const float* a,
                          __global const float* b,
                          __global float* result,
                          const unsigned int count) {
    // Get the global thread ID
    int id = get_global_id(0);

    // Check to ensure we do not go out of bounds
    if (id < count) {
        result[id] = a[id] + b[id];
    }
}

