// Copyright (c)       2015 Patrick Diehl
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <hpx/hpx_main.hpp>
#include <hpx/include/iostreams.hpp>
#include <hpx/lcos/future.hpp>

#include "../../cuda.hpp"

#include <unistd.h>

#define DEBUG 
#define SIZE 100

using namespace hpx::cuda;

static const char kernel_src[] =
		        "                                                                           "
				"extern \"C\"  __global__ void sum(int* array, int* count){ 			  \n"
				" for (int i = blockDim.x * blockIdx.x + threadIdx.x;					  \n"
				"         i < 100;														  \n"
				"         i += gridDim.x * blockDim.x)									  \n"
				"    {				printf(\"%d %d \" , i, array[i] );											  \n"
				"        atomicAdd(&(count[0]), array[i]);							      \n"
				"    }	 //	count[0] = 45;									  \n"
				"}                                             							  \n";



// hpx_main, is the actual main called by hpx
int main(int argc, char* argv[]) {

	//Vector for all futures for the data management
	std::vector<hpx::lcos::future<void>> data_futures;

	// Get list of available Cuda Devices.
	std::vector<device> devices = get_all_devices(2, 0).get();

	// Check whether there are any devices
	if (devices.size() < 1) {
		hpx::cerr << "No CUDA devices found!" << hpx::endl;
		return hpx::finalize();
	}

	// Generate Input data
	int* inputData;
	cudaMallocHost((void**)&inputData, sizeof(int)*SIZE);

	// Create a device component from the first device found
	device cudaDevice = devices[0];

	for (unsigned int i = 0; i < SIZE; i++)
		inputData[i] = 100;

	// Create a buffer
	buffer outbuffer = cudaDevice.create_buffer_sync(SIZE * sizeof(int));

	// Copy input data to the buffer
	data_futures.push_back(outbuffer.enqueue_write(0, SIZE * sizeof(int), inputData));

	// Create the hello_world device program
	program prog = cudaDevice.create_program_with_source(kernel_src).get();

	// Add compiler flags for compiling the kernel

	std::vector<std::string> flags;
	std::string mode = "--gpu-architecture=compute_";
	mode.append(
			std::to_string(cudaDevice.get_device_architecture_major().get()));

	mode.append(
			std::to_string(cudaDevice.get_device_architecture_minor().get()));

	flags.push_back(mode);

	// Compile the program

#ifdef DEBUG
	prog.build(flags, 1);
#else
	prog.build(flags);
#endif

	// Create the buffer for the result
	int* result;
	cudaMallocHost((void**)&result,sizeof(int));
	result[0] = 0;
	buffer resbuffer = cudaDevice.create_buffer_sync(sizeof(int));
	data_futures.push_back(resbuffer.enqueue_write(0,sizeof(int), result));

	//Generate the grid and block dim
    hpx::cuda::server::program::Dim3 grid;
    hpx::cuda::server::program::Dim3 block;

    //Set the values for the grid dimension
    grid.x = 1;
    grid.y = 1;
    grid.z = 1;

    //Set the values for the block dimension
    block.x = 32 ;
    block.y = 1;
    block.z = 1;

	//Set the parameter for the kernel, have to be the same order as in the definition
	std::vector<hpx::cuda::buffer>args;
		args.push_back(outbuffer);
		args.push_back(resbuffer);

	hpx::wait_all(data_futures);

	//Run the kernel at the default stream
	auto kernel_future = prog.run(args,"sum",grid,block);

	hpx::wait_all(kernel_future);

	//Copy the result back
	int* res = resbuffer.enqueue_read_sync<int>(0,sizeof(int));



	hpx::cout << res[0] << hpx::endl;

	return EXIT_SUCCESS;
}

