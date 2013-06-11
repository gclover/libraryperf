

#include <vector>
#include <iostream>

#include <Poco/SharedPtr.h>
#include <Poco/Thread.h>

#include "processor.h"

typedef std::string ConnectionQueue;

class MockWorker : public Worker {
public:
	MockWorker(int id, ConnectionQueue& queue, Processor& processor) 
	
	: id_(id) {}

	virtual void notifyTaskComplete() {
			//std::cout << id_ << " task complete" << std::endl;
	}
private:
	int id_;
};


int main(int argc, char* argv[]) {


	ConnectionQueue queue;

	std::vector<Poco::SharedPtr<Worker> > workers;
	int numWorkers = 4;
	int parallelCap = 10;

	Processor processor(workers, numWorkers, parallelCap);		

	for (int i = 0; i < numWorkers; i++) {
			Poco::SharedPtr<Worker> worker(new MockWorker(i, queue, processor));
			workers.push_back(worker);
	}
	
	Poco::Thread thread;
	thread.start(processor);

	for (int i = 0; i < 10000000; i++) {
			ProcessTask task;
			task.outQueueId = i % numWorkers;
			task.number = i +1;
			processor.getInQueue().push(task);
	}
	processor.stop();

	thread.join();

	return 0;
}
