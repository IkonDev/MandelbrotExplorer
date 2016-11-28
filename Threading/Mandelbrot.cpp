#include "Mandelbrot.h"
#include <assert.h>

void Mandelbrot::MandelBrot(float ax,
							float ay,
							BitmapData& data)
{
	float hWidth  = m_ScreenInfo.screenX / 2;
	float hHeight = m_ScreenInfo.screenY / 2;

	ComplexNumber C = ComplexNumber(1.5 * (ax - hWidth)  / (0.5 * ZoomFactor * m_ScreenInfo.screenX) + xPoint,  //Real
									      (ay - hHeight) / (0.5 * ZoomFactor * m_ScreenInfo.screenY) + yPoint); //Imaginary
	int iteration = 0;

	ComplexNumber Z = C;

	while (iteration < MaxIterations)
	{
		Z = (Z * Z) + C;
		if (Z.Norm() > 4)
		{
			break;
		}
		++iteration;
	}
	/*
	double C_re = 1.5 * (ax - hWidth) / (0.5 * ZoomFactor * m_SI.screenX) + xPoint;  //Real
	double C_im = (ay - hHeight) / (0.5 * ZoomFactor * m_SI.screenY) + yPoint; //Imaginary
	
	double Z_re = C_re;
	double Z_im = C_im;

	int iteration = 0;
	while (iteration < MaxIterations)
	{
		//MakronBrot 1
		Z_im = ((Z_re * Z_im) + (Z_im * Z_re)) + (Z_im + C_im);
		Z_re = ((Z_re * Z_re) - (Z_im * Z_im)) + (Z_re + C_re);

		//MakronBrot 2
		double Z_im2 = Z_im*Z_im;
		Z_re = Z_re*Z_re - Z_im2 + C_re;
		Z_im = 2 * Z_re*Z_im + C_im;

		if (Z_re*Z_re + Z_im*Z_im > 4)
		{
			break;
		}
		++iteration;
	}
	*/
	//Drawing
	UINT* pixels = (UINT*)data.Scan0;
	if (iteration < MaxIterations)
	{
		pixels[((int)ay * (data.Stride / sizeof(int)) + (int)ax)] = Palette[iteration % PaletteSize];
	}
	else
	{
		pixels[((int)ay * (data.Stride / sizeof(int)) + (int)ax)] = 0xff000000;
	}
}

void Mandelbrot::CreatePalette()
{
	double H = (rand() % 360);
	double L = 0;
	for (int P = 0; P < PaletteSize; ++P)
	{
		ColorRGB C(0,0,0);
		C = HSVtoRBG(H, 0.95, L);
		unsigned char cAlpha = 255;
		unsigned char cRed =   (unsigned char)(C.R);
		unsigned char cGreen = (unsigned char)(C.G);
		unsigned char cBlue =  (unsigned char)(C.B);

		unsigned int uiAlpha = 0x00000000;
		unsigned int uiRed = 0x00000000;
		unsigned int uiGreen = 0x00000000;
		unsigned int uiBlue = 0x00000000;

		uiAlpha = (unsigned char)cAlpha << 24;
		uiRed =   (unsigned char)cRed   << 16;
		uiGreen = (unsigned char)cGreen << 8;
		uiBlue =  (unsigned char)cBlue;


		Palette[P] = uiAlpha | uiRed | uiGreen | uiBlue;
		H += (1.0f / (float)PaletteSize) * 360;
		if (H >= 359)
		{
			H = 0;
		}
		L += (1.0f / (float)PaletteSize);
	}
}

void Mandelbrot::Process()
{
	fBitMap->LockBits(&Rect(0, 0, m_ScreenInfo.screenX, m_ScreenInfo.screenY), ImageLockModeWrite, PixelFormat32bppARGB, &bData);
	if(GPUEnabled)
	{
	
	
		cl_int Result = 0;
	
		//m_PixelBuffer = clCreateBuffer(m_Context, CL_MEM_WRITE_ONLY | CL_MEM_HOST_PTR,
		//							   ImageSize, &bData.Scan0, &Result);
	
	
	
	
		Result = clSetKernelArg(Fract->m_Kernel, 0, sizeof(double), &ZoomFactor);
		if (Result != CL_SUCCESS)
		{
			Debug::Log("clSetKernelArg Failed!");
			Debug::Log(Result);
		}
	
		Result = clSetKernelArg(Fract->m_Kernel, 1, sizeof(double), &xPoint);
		if (Result != CL_SUCCESS)
		{
			Debug::Log("clSetKernelArg Failed!");
			Debug::Log(Result);
		}
	
		Result = clSetKernelArg(Fract->m_Kernel, 2, sizeof(double), &yPoint);
		if (Result != CL_SUCCESS)
		{
			Debug::Log("clSetKernelArg Failed!");
			Debug::Log(Result);
		}
	
		cl_event mEvent;
	
		//for (int y = 0; y < m_SI.screenX; ++y)
		//{
		//	Result = clSetKernelArg(Fract->m_Kernel, 1, sizeof(int), &y);
		//	if (Result != CL_SUCCESS)
		//	{
		//		Debug::Log("clSetKernelArg Failed!");
		//		Debug::Log(Result);
		//	}
		//	/*for (int x = 0; x < m_SI.screenX; ++x)
		//	{
		//		Result = clSetKernelArg(Fract->m_Kernel, 0, sizeof(int), &x);
		//		if (Result != CL_SUCCESS)
		//		{
		//			Debug::Log("clSetKernelArg Failed!");
		//			Debug::Log(Result);
		//		}
		//	}*/
		//}
		size_t gWorkOffset[2] = { 0, 0 };

		size_t gWorkSize[2] = { m_ScreenInfo.screenX, m_ScreenInfo.screenY };
		size_t lWorkSize[2] = { m_ScreenInfo.screenX / 4, 1 };
		clEnqueueNDRangeKernel(m_Queue, Fract->m_Kernel, 2, gWorkOffset, gWorkSize, lWorkSize, 0, nullptr, &mEvent);
		size_t ImageSize = sizeof(unsigned int) * (m_ScreenInfo.screenX * m_ScreenInfo.screenY);
		Result = clEnqueueReadBuffer(m_Queue, m_PixelBuffer, CL_TRUE, 0, ImageSize, bData.Scan0, 1, &mEvent, nullptr);
		if (Result != CL_SUCCESS)
		{
			Debug::Log("clEnqueueReadBuffer Failed!");
			Debug::Log(Result);
		}
	
		clFlush(m_Queue);
		clFinish(m_Queue);

		cl_ulong TimeStart = 0;
		Result = clGetEventProfilingInfo(mEvent, CL_PROFILING_COMMAND_START,
										 sizeof(cl_ulong), &TimeStart, nullptr);
		if (Result != CL_SUCCESS)
		{
			printf("clGetEventProfilingInfo failed: %i\n", &Result);
		}

		cl_ulong TimeEnd = 0;
		Result = clGetEventProfilingInfo(mEvent, CL_PROFILING_COMMAND_END,
										 sizeof(cl_ulong), &TimeEnd, nullptr);
		if (Result != CL_SUCCESS)
		{
			printf("clGetEventProfilingInfo failed: %i\n", &Result);
		}

		// return time is in nanoseconds, so convert to seconds
		RenderTime = (TimeEnd - TimeStart) * 1.0e-9;

		DoProc = false;
	}
	else
	{
		double StartTime = glfwGetTime();

		for (auto i = 0; i < NumberOfCores; ++i)
		{
			//Calculate chunk size
			int Low = (i * m_ScreenInfo.ChunkWidth);
			int High = ((i + 1) * m_ScreenInfo.ChunkWidth);

			//Push Threads
			ThreadList.push_back(std::thread(
				[=]()
			{
				for (int y = 0; y < m_ScreenInfo.screenY; ++y)
				{
					for (auto x = Low; x < High; ++x)
					{
						MandelBrot(x, y, bData);
					}
				}
			}
			));
		}

		for (int T = 0; T < ThreadList.size(); ++T)
		{
			ThreadList[T].join();
		}
		ThreadList.clear();

		double NextTime = glfwGetTime();

		RenderTime = (NextTime - StartTime);
	}
	fBitMap->UnlockBits(&bData);

}

ColorRGB Mandelbrot::HSVtoRBG(int H, float S, float V)
{
	float r, g, b;
	float C = V * S;
	float X = C * (1 - abs((H / 60) % 2 - 1));
	float m = V - C;
	switch (H / 60) {
	case 0: r = C, g = X, b = 0; break;
	case 1: r = X, g = C, b = 0; break;
	case 2: r = 0, g = C, b = X; break;
	case 3: r = 0, g = X, b = C; break;
	case 4: r = X, g = 0, b = C; break;
	case 5: r = C, g = 0, b = X; break;
	}

	return ColorRGB((r+m) * 255, (g+m) * 255, (b+m) * 255);
}

void Mandelbrot::GetUserInfo()
{
	std::cout << "Set screen X size: ";
	std::cin >> m_ScreenInfo.screenX;

	std::cout << "Set screen Y size: ";
	std::cin >> m_ScreenInfo.screenY;

	m_ScreenInfo.screenSize = m_ScreenInfo.screenX * m_ScreenInfo.screenY;

	m_ScreenInfo.ChunkSize  = m_ScreenInfo.screenSize / NumberOfCores;
	m_ScreenInfo.ChunkWidth = m_ScreenInfo.screenX    / NumberOfCores;

	std::cout << "Enable GPU Processing? ";

	char Key;
	Key = _getch();

	if (Key == 121)
	{
		std::cout << "\nGPU Processing Enabled!";
		GPUEnabled = true;
	}
	else
	{
		std::cout << "\nGPU Processing Disabled!";
		GPUEnabled = false;
	}
	Sleep(250);
}

void Mandelbrot::Startup()
{
	//Get Cores
	NumberOfCores = std::thread::hardware_concurrency();

	//Initialize GLFW for Timing
	//NOTE: REPLACE THIS WITH A BETTER TIMING FUNCTION (CHECK CACHE COHERENCY LECTURE)
	glfwInit();

	//Seed std::rand
	srand(0);

	//Create the ScreenInfo and fill it
	m_ScreenInfo = ScreenInfo();
	GetUserInfo();

	//Create palette
	CreatePalette();

	//Create GDI & Startup
	GdiplusStartup(&GDI, &GdiplusStartupInput(), NULL);

	//Get a console handle
	ConsoleWin = GetConsoleWindow();
	ConDC = GetDC(ConsoleWin);
	graphics = new Graphics(ConDC);
	
	//Create a Bitmap
	fBitMap = new Bitmap(m_ScreenInfo.screenX, m_ScreenInfo.screenY, PixelFormat32bppARGB);

	if (GPUEnabled)
	{
		//Gain Access to the platform
		//Arg = Num of platforms, Array to platformIDs, uint* for each platform (NULLPTR because we only want 1)
		assert(clGetPlatformIDs(1, &m_Platform, nullptr) == CL_SUCCESS);

		//Request devices
		//Arg = Platform, device type, number of devices, device array, uint* for num of devices
		assert(clGetDeviceIDs(m_Platform, CL_DEVICE_TYPE_GPU, 1, &m_Device, NULL) == CL_SUCCESS);

		//Create a device context and Command Queue
		cl_context_properties contextProperties[] =
		{
			CL_CONTEXT_PLATFORM, (cl_context_properties)m_Platform, 0
		};
		cl_int Result;
		m_Context = clCreateContext(contextProperties, 1, &m_Device, nullptr, nullptr, &Result);
		if (Result != CL_SUCCESS)
		{
			Debug::Log("clCreateContext Failed!");
			Debug::Log(Result);
		}

		Result = 0;
		m_Queue = clCreateCommandQueue(m_Context, m_Device, CL_QUEUE_PROFILING_ENABLE, &Result);
		if (Result != CL_SUCCESS)
		{
			Debug::Log("clCreateCommandQueue Failed!");
			Debug::Log(Result);
		}

		Fract = new Kernel();
		Fract->LoadFromFile("Data/Kernels/Mandelbrot.cl");

		kProgram = new KernelProgram();

		kProgram->CreateProgram(m_Context, Fract);
		kProgram->CompileProgram(&m_Device);

		Fract->ExtractKernel(kProgram->m_KernelProgram, "Mandelbrot");

		fBitMap->LockBits(&Rect(0, 0, m_ScreenInfo.screenX, m_ScreenInfo.screenY), ImageLockModeWrite, PixelFormat32bppARGB, &bData);
		//Scan0
		size_t ImageSize = sizeof(unsigned int) * (m_ScreenInfo.screenX * m_ScreenInfo.screenY);
		m_PixelBuffer = clCreateBuffer(m_Context, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR,
			ImageSize, bData.Scan0, &Result);
		if (Result != CL_SUCCESS)
		{
			Debug::Log("clCreateBuffer Failed!");
			Debug::Log(Result);
		}

		Result = clSetKernelArg(Fract->m_Kernel, 3, sizeof(cl_mem), &m_PixelBuffer);
		if (Result != CL_SUCCESS)
		{
			Debug::Log("clSetKernelArg Failed!");
			Debug::Log(Result);
		}

		Result = clSetKernelArg(Fract->m_Kernel, 4, sizeof(int), &bData.Stride);
		if (Result != CL_SUCCESS)
		{
			Debug::Log("clSetKernelArg Failed!");
			Debug::Log(Result);
		}

		//pallete
		m_Palette = clCreateBuffer(m_Context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR,
									sizeof(unsigned int) * PaletteSize, Palette, &Result);
		if (Result != CL_SUCCESS)
		{
			Debug::Log("clCreateBuffer Failed!");
			Debug::Log(Result);
		}

		Result = clSetKernelArg(Fract->m_Kernel, 5, sizeof(cl_mem), &m_Palette);
		if (Result != CL_SUCCESS)
		{
			Debug::Log("clSetKernelArg Failed!");
			Debug::Log(Result);
		}

		int pSize = PaletteSize;
		Result = clSetKernelArg(Fract->m_Kernel, 6, sizeof(int), &pSize);
		if (Result != CL_SUCCESS)
		{
			Debug::Log("clSetKernelArg Failed!");
			Debug::Log(Result);
		}

		Result = clSetKernelArg(Fract->m_Kernel, 7, sizeof(unsigned int), &MaxIterations);
		if (Result != CL_SUCCESS)
		{
			Debug::Log("clSetKernelArg Failed!");
			Debug::Log(Result);
		}
	}
	fBitMap->UnlockBits(&bData);



}

void Mandelbrot::Shutdown()
{
	delete fBitMap;
	graphics->ReleaseHDC(ConDC);
}

void Mandelbrot::Draw()
{
	system("cls");
	Color cCol = Color::Red;

	float hWidth =  m_ScreenInfo.screenX / 2;
	float hHeight = m_ScreenInfo.screenY / 2;

	//Draw a cursor pixel
	//Y = -1
	fBitMap->SetPixel(hWidth  - 1,
					  hHeight - 1,
					  cCol);

	fBitMap->SetPixel(hWidth,
					  hHeight - 1,
					  cCol);

	fBitMap->SetPixel(hWidth  + 1,
					  hHeight - 1,
					  cCol);
	//Y = 0
	fBitMap->SetPixel(hWidth  - 1,
					  hHeight,
					  cCol);

	fBitMap->SetPixel(hWidth,
					  hHeight,
					  cCol);

	fBitMap->SetPixel(hWidth  + 1,
					  hHeight,
					  cCol);
	//Y = 1
	fBitMap->SetPixel(hWidth  - 1,
					  hHeight + 1,
					  cCol);

	fBitMap->SetPixel(hWidth,
					  hHeight + 1,
					  cCol);

	fBitMap->SetPixel(hWidth  + 1,
					  hHeight + 1,
					  cCol);
	
	graphics->DrawImage(fBitMap, Rect(0, 0, m_ScreenInfo.screenX, m_ScreenInfo.screenY));

	//std::cout.precision(15);
	//std::cout << "Point X: " << xPoint << std::endl;
	//std::cout << "Point Y: " << yPoint << std::endl;
	//std::cout << "Zoom: "       << ZoomFactor    << std::endl;
	//std::cout << "Iterations: " << MaxIterations << std::endl;
	//std::cout << RenderTime << " Seconds Elapsed" << std::endl;
}

bool Mandelbrot::TakeInput()
{
	cl_int Result;

	char Key;
	Key = _getch();
	
	if (Key != 0)
	{
		if (Key == 45) // Minus
		{
			ZoomFactor /= 1.001 * zSpeed;
		}
		else if (Key == 61) //Plus
		{
			ZoomFactor *= 1.001 * zSpeed;
		}
		else if (Key == 75) //Left
		{
			xPoint -= (0.05f / ZoomFactor);
		}
		else if (Key == 77) //Right
		{
			xPoint += (0.05f / ZoomFactor);
		}
		else if (Key == 72) //Up
		{
			yPoint -= (0.05f / ZoomFactor);
		}
		else if (Key == 80) //Down
		{
			yPoint += (0.05f / ZoomFactor);
		}
		else if (Key == 95)//Shift + Minus
		{
			MaxIterations--;
			if (GPUEnabled)
			{
				Result = clSetKernelArg(Fract->m_Kernel, 7, sizeof(int), &MaxIterations);
				if (Result != CL_SUCCESS)
				{
					Debug::Log("clSetKernelArg Failed!");
					Debug::Log(Result);
				}
			}
		}
		else if (Key == 43)//Shift + Plus
		{
			MaxIterations++;
			if (GPUEnabled)
			{
				Result = clSetKernelArg(Fract->m_Kernel, 7, sizeof(int), &MaxIterations);
				if (Result != CL_SUCCESS)
				{
					Debug::Log("clSetKernelArg Failed!");
					Debug::Log(Result);
				}
			}
		}
		else if (Key == 112)//p
		{
			srand(rand() % 1024);
			CreatePalette();
			if (GPUEnabled)
			{
				Result = clEnqueueWriteBuffer(m_Queue, m_Palette, CL_TRUE, 0, sizeof(unsigned int) * PaletteSize, Palette, 0, nullptr, nullptr);
				if (Result != CL_SUCCESS)
				{
					Debug::Log("clEnqueueWriteBuffer Failed!");
					Debug::Log(Result);
				}
			}
			Process();
		}
		else if (Key == 105)//i
		{
			if (GPUEnabled)
			{
				GPUEnabled = false;
				system("cls");
				Debug::Log("GPU DISABLED");
				Sleep(400);
				system("cls");
			}
			else
			{
				GPUEnabled = true;
				system("cls");
				Debug::Log("GPU ENABLED");
				Sleep(400);
				system("cls");
			}
		}


		
		return true;
	}

	return false;
}

