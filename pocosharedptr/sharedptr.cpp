#include <string>
#include <iostream>

#include <Poco/SharedPtr.h>

#include "progresstimer.h"

using Poco::SharedPtr;

class Cls {
public:
	Cls() { 
		// std::cout << "construct" << std::endl; 
	}

	~Cls() { 
		// std::cout << "~construct" << std::endl; 
	}
};


int main(int argc, char* argv[])
{
	ProgressTimer timer;

	for (int i = 0; i < 10000000; ++i) {
		SharedPtr<Cls> p1(new Cls());
	}
	return 0;
}
