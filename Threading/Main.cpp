#include <thread>
#include "Mandelbrot.h"

int main()
{
	Mandelbrot M = Mandelbrot();

	M.Startup();
	while (true)
	{
		if (M.DoProc)
		{
			M.Process();
		}
		M.Draw();
		if (M.TakeInput())
			M.DoProc = true;
	}
	M.Shutdown();
	return 0;

}