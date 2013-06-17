
#include <Poco/Thread.h>
#include <Poco/Runnable.h>
#include <Poco/ThreadPool.h>
#include <Poco/Util/ServerApplication.h>
#include <tbb/tick_count.h>
#include <tbb/concurrent_queue.h>
#include <iostream>

using Poco::Thread;
using Poco::Runnable;
using Poco::ThreadPool;
using Poco::Util::Application;
using Poco::Util::ServerApplication;

typedef tbb::concurrent_bounded_queue<int> JobQueue;

static const int numpack = 1000000;

class First : public Runnable {
public:
	First(JobQueue* queue) : queue_(queue) {
	} 

	void run() {
			for (int i = 0; i < numpack; ++i) {
					queue_->push(i);
			}
	}
private:
	JobQueue* queue_;
};

class Middle : public Runnable {
public:
	Middle(JobQueue* prev, JobQueue* next) : prev_(prev), next_(next) {
	}

	void run() {
			int v;
			while (true) {
				prev_->pop(v);
				next_->push(v);
			}
	}

	JobQueue* prev_;
	JobQueue* next_;
};

class Last : public Runnable {
public:	
	Last(JobQueue* queue) : queue_(queue) {
	}

	void run() {
			int v;
			tbb::tick_count start;
			tbb::tick_count end;
			while (true) {
				queue_->pop(v);
					if (v == 0)
						start = tbb::tick_count::now();
					if (v == (numpack-1)) {
						end = tbb::tick_count::now();
						std::cout << (end - start).seconds() << std::endl;
					}
			}
	}
private:
	JobQueue* queue_;
};


int main(int argc, char* argv[]) {

	JobQueue queue;
	
	ThreadPool threads;

	int numqueue = 10;
	
	std::vector<JobQueue> queues(numqueue);
	std::vector<Middle> ms;
	First f(&queues[0]);
	Last l(&queues[numqueue-1]);
	for (int i = 0; i < (numqueue-1); ++i) {
			ms.push_back(Middle(&queues[i], &queues[i+1]));
	}

	for (int i = 0; i < (numqueue-1); ++i) {
			threads.start(ms[i]);
	}
	threads.start(l);
	threads.start(f);

	threads.joinAll();

	return Application::EXIT_OK;
}



