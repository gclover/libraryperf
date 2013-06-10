#ifndef PROGRESSTIMER_H
#define PROGRESSTIMER_H

#include <tbb/tick_count.h>
#include <iostream>

class ProgressTimer {
public:
	ProgressTimer() {
		start_ = tbb::tick_count::now();
	}

	~ProgressTimer() {
		std::cout << elapsed() << std::endl;
	}

	void start() {
		start_ = tbb::tick_count::now();
	}

	double elapsed() {
		tbb::tick_count now = tbb::tick_count::now();
		return (now - start_).seconds();
	}

private:
	tbb::tick_count start_;	
};


#endif
