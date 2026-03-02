#include "CL/cl.h"
#include <cassert>
#include <cstdio>
#include <string>
#include <sstream>
#include <fstream>
int main()
{
	cl_platform_id platforms[10];
	cl_device_id dev = 0;
	cl_uint n_platforms;
	int err = clGetPlatformIDs(10, platforms, &n_platforms);
	printf("%d %d\n", err, n_platforms);
	err = clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_GPU, 1, &dev, nullptr);
	printf("%d\n", err);
	printf("%x\n", dev);
	
	cl_context ctx = clCreateContext(nullptr, 1, &dev, nullptr, nullptr, &err);
	printf("%d\n", err);
	std::string source;
	std::ifstream ifs("kernel.cl");
	std::stringstream buf; buf << ifs.rdbuf(); source = std::move(buf).str();
	const char* srcptr = source.data();
	size_t srclen = source.size();
	cl_program prog = clCreateProgramWithSource(ctx, 1, &srcptr, &srclen, &err);
	printf("%d\n", err);
	err = clBuildProgram(prog, 0, nullptr, nullptr, nullptr, nullptr);
	printf("%d\n", err);
	if(err != 0) {
		printf("Build error\n");
		size_t log_sz;
		clGetProgramBuildInfo(prog, dev, CL_PROGRAM_BUILD_LOG, 0, nullptr, &log_sz);
		std::string log(log_sz, '\0');
		clGetProgramBuildInfo(prog, dev, CL_PROGRAM_BUILD_LOG, log_sz, log.data(), nullptr);
		printf("%s\n", log.data());
		return 1;
	}
	cl_kernel kernel = clCreateKernel(prog, "bruteforce", &err);
	printf("%d\n", err);

	size_t overall = 1ull << 48;
	size_t global_work = 1ull << 31;
	size_t local_work = 256;
	for(size_t i = 0; i < overall / global_work; i++) {
		uint64_t offset = i * global_work;
		cl_command_queue queue = clCreateCommandQueue(ctx, dev, 0, &err);
		assert(err == 0);
		//printf("%d\n", err);
		err = clSetKernelArg(kernel, 0, sizeof(offset), &offset);
		assert(err == 0);
		//printf("%d\n", err);
		err = clEnqueueNDRangeKernel(queue, kernel, 1, nullptr, &global_work, &local_work, 0, nullptr, nullptr);
		assert(err == 0);
		//printf("%d\n", err);
		err = clFinish(queue);
		assert(err == 0);
		//printf("%d\n", err);
		err = clReleaseCommandQueue(queue);
		assert(err == 0);
	}
}
