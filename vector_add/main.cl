__kernel void add(__global double* a, __global double* b, __global double* results)
{
    int i = get_global_id(0);
    results[i] = a[i] + b[i];
}
