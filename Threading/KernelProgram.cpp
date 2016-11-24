#include "KernelProgram.h"

#include <MAKRON/Debug.h>
#include <iostream>
bool Kernel::LoadFromFile(const char* File)
{
	//open the file and check for an early out
	FILE* file = new FILE();
	fopen_s(&file, File, "rb");

	if (file == nullptr)
	{
		return 0;
	}

	//Get size of kernel
	fseek(file, 0, SEEK_END);
	KernelSize = ftell(file);

	//Write it to a char*
	fseek(file, 0, SEEK_SET);
	KernelSource = new char[KernelSize + 1];
	memset(KernelSource, 0, KernelSize + 1);
	fread(KernelSource, sizeof(char), KernelSize, file);

	//close the file
	fclose(file);
}

void Kernel::ExtractKernel(cl_program KernelProgram, const char * KernelFuncName)
{
	cl_int Result;
	m_Kernel = clCreateKernel(KernelProgram, KernelFuncName, &Result);
	if (Result != CL_SUCCESS)
	{
		Debug::Log("clCreateKernel Failed!");
		Debug::Log(Result);
	}
}

void Kernel::ReleaseKernel()
{
	clReleaseKernel(m_Kernel);
}

void KernelProgram::CreateProgram(cl_context Context, Kernel* _Kernel)
{
	cl_int Result;
	m_KernelProgram = clCreateProgramWithSource(Context, 1, 
		(const char**)&_Kernel->KernelSource, 
		(const size_t*)&_Kernel->KernelSize, 
		&Result);

	if (Result != CL_SUCCESS)
	{
		Debug::Log("clCreateProgram Failed!");
		Debug::Log(Result);
	}
}

void KernelProgram::CompileProgram(cl_device_id* Device)
{
	cl_int Result = clBuildProgram(m_KernelProgram, 1, Device, nullptr, nullptr, nullptr);
	if (Result != CL_SUCCESS)
	{
		Debug::Log("clBuildProgram Failed!");
		Debug::Log(Result);
	}
	if (Result == CL_BUILD_PROGRAM_FAILURE) 
	{
		// Determine the size of the log
		size_t log_size;
		clGetProgramBuildInfo(m_KernelProgram, *Device, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);

		// Allocate memory for the log
		char *log = (char *)malloc(log_size);

		// Get the log
		clGetProgramBuildInfo(m_KernelProgram, *Device, CL_PROGRAM_BUILD_LOG, log_size, log, NULL);

		// Print the log
		Debug::Log(log);
	}
}

void KernelProgram::ReleaseProgram()
{
	clReleaseProgram(m_KernelProgram);
}

