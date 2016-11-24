#pragma once
#include <CL/cl.h>

class Kernel
{
public:
	bool LoadFromFile(const char* File);
	void ExtractKernel(cl_program KernelProgram, const char* KernelFuncName);
	void ReleaseKernel();

	cl_kernel m_Kernel;

	char* KernelSource;
	unsigned int KernelSize = 0;
};

class KernelProgram
{
public:
	void CreateProgram(cl_context Context, Kernel* _Kernel);
	void CompileProgram(cl_device_id* Device);
	void ReleaseProgram();

	cl_program m_KernelProgram;
};