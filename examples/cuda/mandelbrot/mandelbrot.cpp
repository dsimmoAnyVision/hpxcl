// Copyright (c)       2017 Madhavan Seshadri
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <hpx/hpx_main.hpp>
#include <hpx/include/iostreams.hpp>
#include <hpx/lcos/future.hpp>
#include <hpx/hpx_main.hpp>

#include <hpxcl/cuda.hpp>

//for writing image
#include "examples/opencl/mandelbrot/pngwriter.cpp"

using std::atoi;
using namespace hpx::cuda;

//###########################################################################
//Main
//###########################################################################

int main(int argc, char* argv[]){

	//Vector for all futures for the data management
	std::vector<hpx::lcos::future<void>> data_futures;

	//Reading a list of available devices in hpx locality
	//Returns a future
	std::vector<device> devices = get_all_devices(2, 0).get();

	if(devices.size() < 1) {
		hpx::cerr << "No CUDA devices found! Terminating...." << hpx::endl;
		return hpx::finalize();
	}
	
	if (argc != 4) {
		std::cout << "Usage: " << argv[0] << " #iterations width height"<< std::endl;
		exit(1);
	}

	int width = atoi(argv[2]);
	int height = atoi(argv[3]);
	int iterations = atoi(argv[1]);
	const int bytes = sizeof(char) * width * height * 3;

	//Malloc Host
	char *image;
	cudaMallocHost((void**) &image, bytes);
	//memset(in, 0, bytes);

	int deviceCount = devices.size();
	int numDevices = std::min(4, deviceCount);

	char *mainImage = (char*)malloc(bytes);

	std::vector<hpx::cuda::buffer> args;
	//Generate the grid and block dim
	hpx::cuda::server::program::Dim3 grid;
	hpx::cuda::server::program::Dim3 block;

	block.x = 16;
	block.y = 16;
	block.z = 1;

	grid.x = width / block.x;
	grid.y = height / (block.y*numDevices);
	grid.z = 1;

	std::vector<hpx::future<void>> progBuildVector;
	std::vector<program> progVector;
	std::vector<device> deviceVector;

	//creating vector of futures
	std::vector<hpx::future<void>> kernelFutures;

	for(int i = 0;i<numDevices;i++)
	{ 
		//Creating new devices array from the list of devices
		device cudaDevice = devices[i];

		//Create a Mandelbrot device program
		program prog = cudaDevice.create_program_with_file("kernel.cu");

		//Compile with the kernal
		std::vector<std::string> flags;
		std::string mode = "--gpu-architecture=compute_";
		mode.append(
			std::to_string(cudaDevice.get_device_architecture_major().get()));
		mode.append(
			std::to_string(cudaDevice.get_device_architecture_minor().get()));

		flags.push_back(mode);

		progBuildVector.push_back(prog.build(flags, "kernel"));
		progVector.push_back(prog);
		deviceVector.push_back(cudaDevice);
	}

	//wait for program to build on all devices
	hpx::wait_all(progBuildVector);

	//Image Buffer Vector
	std::vector<buffer> imageBufferVector;

	for (int i = 0; i < numDevices; i++)
	{
		//calculate the start position
		int yStart = i*height / numDevices;
		
		device cudaDevice = deviceVector.at(i);
		program prog = progVector.at(i);
		
		//creating buffers
		buffer imageBuffer = cudaDevice.create_buffer(bytes/numDevices);
		buffer widthBuffer = cudaDevice.create_buffer(sizeof(int));
		buffer heightBuffer = cudaDevice.create_buffer(sizeof(int));
		buffer iterationsBuffer = cudaDevice.create_buffer(sizeof(int));
		buffer yStartBuffer = cudaDevice.create_buffer(sizeof(int));

		// Copy input data to the buffer
		data_futures.push_back(imageBuffer.enqueue_write(0, bytes, image));
		data_futures.push_back(widthBuffer.enqueue_write(0, sizeof(int), &width));
		data_futures.push_back(heightBuffer.enqueue_write(0, sizeof(int), &height));
		data_futures.push_back(iterationsBuffer.enqueue_write(0, sizeof(int), &iterations));
		data_futures.push_back(yStartBuffer.enqueue_write(0, sizeof(int), &yStart));

		args.push_back(imageBuffer);
		args.push_back(widthBuffer);
		args.push_back(heightBuffer);
		args.push_back(iterationsBuffer);
		args.push_back(yStartBuffer);

		imageBufferVector.push_back(imageBuffer);

		//run the program on the device
		kernelFutures.push_back(prog.run(args, "kernel", grid, block, args));
		//for multiple runs
		args.clear();
	}

	//wait for all the kernal futures to return
	hpx::wait_all(kernelFutures);

	//Stich multiple images
	for (int i = 0; i < numDevices; i++)
	{
		int pointer = i*width / numDevices;
		strncpy(&image[pointer],imageBufferVector.at(i).enqueue_read_sync<char>(0, bytes/numDevices), bytes/numDevices);
	}

	//write images to file
	std::shared_ptr<std::vector<char>> img_data;
	img_data = std::make_shared <std::vector <char> >
		(std::vector<char>(image, image + bytes));

	save_png(img_data, width, height, "Mandelbrot_img.png");

	//Free Memory
	cudaFree(image);
	free(mainImage);

	return EXIT_SUCCESS;
}