// minimal sample program using external libraries

#include <iostream>


#include "stlib.h"
#include "shlib1.h"
#include "shlib2.h"

/// This is a Hello World program for running a basic Bullet physics simulation

int main(int argc, char** argv)
{
	std::cout << " --- Begin Lib test program" << std::endl;
	
	std::cout << " --- static library call:" << std::endl;
	std::cout << whoAmI() << std::endl;
	std::cout << " --- dynamic library call:" << std::endl;
	std::cout << dynWhoAmI1() << std::endl;
	std::cout << " --- dynamic library call:" << std::endl;
	std::cout << dynWhoAmI2() << std::endl;
	
	std::cout << " --- End Lib test program" << std::endl;
	
	return 0;
}

