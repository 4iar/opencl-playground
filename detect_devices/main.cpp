#include <CL/cl.hpp>
#include <iostream>
#include <vector>


/**
 * \brief Find all compute devices
 *
 * Example output:
 *
 *   - Clover
 *   - NVIDIA CUDA
 *       > GeForce GT 630M
 *
 *
 */
int main () {
    std::cout << "\n\n-----------------------------------------" << std::endl;
    std::cout << "Example output: " << std::endl;
    std::cout << " - platform" << std::endl;
    std::cout << "     > device associated with platform" << std::endl;
    std::cout << "-----------------------------------------\n\n" << std::endl;


    // Get platforms (e.g. Clover, Nvidia CUDA)
    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);

    // Get available devices for each platform
    for (auto platform = platforms.begin(); platform != platforms.end(); platform++) {
        std::cout << " - " << (*platform).getInfo<CL_PLATFORM_NAME>() << std::endl;

        std::vector<cl::Device> devices;
        (*platform).getDevices(CL_DEVICE_TYPE_ALL, &devices);

        for (auto device = devices.begin(); device != devices.end(); device++) {
            std::cout << "     > " << (*device).getInfo<CL_DEVICE_NAME>() << std::endl;
        }
    }
};
