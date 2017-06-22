#define __CL_ENABLE_EXCEPTIONS


#include <CL/cl.hpp>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <random>
#include <fstream>
#include <cmath>
#include <string>
#include <chrono>

#ifndef DEVICE
#define DEVICE CL_DEVICE_TYPE_DEFAULT
#endif

std::string loadProgram (std::string input);
double secondsElapsed(std::chrono::time_point<std::chrono::high_resolution_clock> start);


int main (int argc, char* argv[]) {
    const int size = atoi(argv[1]);

    std::vector<double> h_results(size);
    std::vector<double> h_a(size);
    std::vector<double> h_b(size);

    // Generate the input vectors based on provided size
    for (int i = 0; i < size; i++) {
        // Get random numbers between 0-1000.
        // This isn't a great way to generate random numbers but is fine
        // for this demo.
        h_a[i] = rand() % 1000;
        h_b[i] = rand() % 1000;
    }

    std::chrono::time_point<std::chrono::high_resolution_clock> start;

    try {
        cl::Context context(DEVICE);

        cl::Program program(context, loadProgram("main.cl"), true);

        auto kernel = cl::make_kernel<cl::Buffer, cl::Buffer, cl::Buffer>(program, "add");

        // last arg (bool readOnly) ->
        // true = CL_MEM_READ_ONLY
        // false = CL_MEM_READ_WRITE
        //
        // bool useHostPtr is the 5th arg and is default false
        // if true, the array is not copied to device memory
        //
        // note these are blocking calls
        cl::Buffer d_a = cl::Buffer(context, begin(h_a), end(h_a), true);
        cl::Buffer d_b = cl::Buffer(context, begin(h_b), end(h_b), true);

        cl::Buffer d_results = cl::Buffer(context, CL_MEM_WRITE_ONLY, sizeof(double) * size);

        start = std::chrono::high_resolution_clock::now();

        cl::CommandQueue queue(context);
        kernel(cl::EnqueueArgs(queue, cl::NDRange(size)), d_a, d_b, d_results);

        queue.finish();

        std::cout << "Parallel execution finished in " << secondsElapsed(start) << " s." << std::endl;

        cl::copy(queue, d_results, h_results.begin(), h_results.end());

    } catch (cl::Error err) {
        std::cerr << "ERROR: " << err.what() << "(" << err.err() << ")" << std::endl;
    }

    start = std::chrono::high_resolution_clock::now();

    std::vector<double> results(size);
    for (int i = 0; i < size; i++) {
        results[i] = h_a[i] * h_b[i];
    }

    std::cout << "Linear execution finished in " << secondsElapsed(start) << " s." << std::endl;
}

/**
 * Pinched from https://github.com/HandsOnOpenCL/Exercises-Solutions/blob/master/Exercises/Cpp_common/util.hpp
 */
std::string loadProgram(std::string input) {
    std::ifstream stream(input.c_str());
    if (!stream.is_open()) {
        std::cout << "Cannot open file: " << input << std::endl;
        exit(1);
    }

    return std::string(std::istreambuf_iterator<char>(stream), (std::istreambuf_iterator<char>()));
}

double secondsElapsed (std::chrono::time_point<std::chrono::high_resolution_clock> start) {
    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = finish - start;
    return elapsed.count();
}
