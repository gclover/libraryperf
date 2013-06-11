
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

class Master : public Runnable {
public:
	Master(JobQueue* queue) : queue_(queue) {
	} 

	void run() {
		std::cout << "master " << std::endl;
		int value;
		while (true) {
			queue_->pop(value);
			if (value == 0)
				break;
		}
	}
private:
	JobQueue* queue_;
};

class Slave : public Runnable {
public:	
	Slave(const std::string& name, JobQueue* queue) : name_(name), queue_(queue) {
	}

	void run() {
		std::cout << "slave " << name_ << std::endl;
		for (int i = 1; i < 1000000; ++i) {
			queue_->push(i);
		}
		queue_->push(0);
		std::cout << "slave exit. " << name_ << std::endl;
	}
private:
	std::string name_;
	JobQueue* queue_;
};


int main(int argc, char* argv[]) {

	tbb::tick_count start = tbb::tick_count::now();

	JobQueue queue;
	
	ThreadPool threads;
	Master master(&queue);
	Slave slave1("1", &queue);
	Slave slave2("2", &queue);
	Slave slave3("3", &queue);

	threads.start(master);
	threads.start(slave1);
	threads.start(slave2);
	threads.start(slave3);

	threads.joinAll();

	tbb::tick_count end = tbb::tick_count::now();

	std::cout << (end - start).seconds() << std::endl;


	return Application::EXIT_OK;
}
