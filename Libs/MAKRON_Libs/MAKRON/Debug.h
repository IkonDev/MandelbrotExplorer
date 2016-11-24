/*=============================
* AUTHOR: John Ikonomou
* DATE: UNK/
* DESCRIPTION: Static Debugging class
=============================*/
#pragma once
#include <iostream>
class Debug
{
public:
	static inline void Debug::Log(char* string)
	{
		std::cout << string << std::endl;
	}
	
	static inline void Debug::Log(std::string string)
	{
		std::cout << string.c_str() << std::endl;
	}

	static inline void Debug::Log(float string)
	{
		std::cout.precision(20);
		std::cout << string << std::endl;
	}

	static inline void Debug::Log(double string)
	{
		std::cout.precision(20);
		std::cout << string << std::endl;
	}

	static inline void Debug::Log(int string)
	{
		std::cout << string << std::endl;
	}
	//With addon string
	static inline void Debug::Log(float string, std::string string2)
	{
		std::cout.precision(20);
		std::cout << string << string2.c_str() << std::endl;
	}

	static inline void Debug::Log(double string, std::string string2)
	{
		std::cout.precision(20);
		std::cout << string << string2.c_str() << std::endl;
	}

	static inline void Debug::Log(int string, std::string string2)
	{
		std::cout << string << string2.c_str() << std::endl;
	}
};

