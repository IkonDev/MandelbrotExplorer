#pragma once

#include <iostream>
#include <iomanip>
#include <conio.h>
#include <thread>
#include <vector>

#include <Windows.h>
#include <Gdiplus.h>
#include <CL/cl.h>
#include <MAKRON/Debug.h>
#include <GLFW/glfw3.h>

#include "KernelProgram.h"


using namespace Gdiplus;

static unsigned int MaxIterations = 512;
#define PaletteSize 64

struct ScreenInfo
{
	unsigned int screenX;
	unsigned int screenY;
	unsigned int screenSize;
	unsigned int ChunkSize;
	unsigned int ChunkWidth;
};

class ComplexNumber
{
public:
	long double Re = 0;
	long double Im = 0;

	ComplexNumber(long double re, long double im)
	{
		Re = re;
		Im = im;
	}

	ComplexNumber ComplexNumber::operator+(ComplexNumber RHS)
	{
		return ComplexNumber(Re + RHS.Re, Im + RHS.Im);
	}

	ComplexNumber ComplexNumber::operator*(ComplexNumber RHS)
	{
		return ComplexNumber(Re * RHS.Re - Im * RHS.Im,
							 Re * RHS.Im + Im * RHS.Re);
	}

	long double ComplexNumber::Norm()
	{
		return Re * Re + Im * Im;
	}
};

struct mRGB
{
	mRGB(unsigned char r, unsigned char g, unsigned char b)
	{
		R = r;
		G = g;
		B = b;
	}
	unsigned char R, G, B;
};

class Mandelbrot
{
public:
	void Startup();
	void Shutdown();
	
	void Process();
	void Draw();

	bool TakeInput();

	unsigned int NumberOfCores = 0;

	bool DoProc = true;

private:

	void MandelBrot(float ax,
					float ay,
					BitmapData& data);

	void CreatePalette();

	mRGB HSVtoRBG(int H, float S, float V);



	void GetUserInfo();

	Bitmap* fBitMap;

	ULONG_PTR GDI;
	HWND ConsoleWin;
	HDC ConDC;
	Graphics* graphics;

	ScreenInfo m_SI;

	unsigned int Palette[PaletteSize];
	BitmapData bData;
	std::vector<std::thread> ThreadList;

	//double ZoomFactor = 1;
	//double xPoint = 0;
	//double yPoint = 0;
	double ZoomFactor = 1;
	double xPoint = 0;
	double yPoint = 0;
	float zSpeed = 2.5f;

	//OpenCL
	cl_platform_id   m_Platform;
	cl_device_id     m_Device;
	cl_context       m_Context;
	cl_command_queue m_Queue;
	cl_mem           m_PixelBuffer;
	cl_mem			 m_Palette;
	double RenderTime;
	Kernel* Fract;
	KernelProgram* kProgram;

	bool GPUEnabled = false;
};

