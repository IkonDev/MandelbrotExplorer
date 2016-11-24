#pragma OPENCL EXTENSION cl_khr_fp64 : enable 

__kernel void Mandelbrot(double ZoomFactor, double xPoint, double yPoint, __global unsigned int* Scan0, int Stride, __global unsigned int* Palette, int PaletteSize, int MaxIterations)
{
	int width =  get_global_size(0);
	int height = get_global_size(1);
	float hWidth =  width / 2;
	float hHeight = height / 2;

	int ax = get_global_id(0);
	int ay = get_global_id(1);

	double C_re = 1.5 * (ax - hWidth) / (0.5 * ZoomFactor * width) + xPoint;  //Real
	double C_im = (ay - hHeight) / (0.5 * ZoomFactor * height) + yPoint; //Imaginary

	double Z_re = C_re;
	double Z_im = C_im;

	int iteration = 0;
	while (iteration < MaxIterations)
	{
		double Z_im2 = Z_im*Z_im;
		Z_im = 2 * Z_re*Z_im + C_im;
		Z_re = Z_re*Z_re - Z_im2 + C_re;

		if (Z_re*Z_re + Z_im*Z_im > 4)
		{
			break;
		}
		++iteration;
	}

	//Drawing
	
	if (iteration < MaxIterations)
	{
		Scan0[(ay * (Stride / sizeof(int)) + ax)] = Palette[iteration % PaletteSize];
	}
	else
	{
		Scan0[(ay * (Stride / sizeof(int)) + ax)] = 0xff000000;
	}
	
	
}