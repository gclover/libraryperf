#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <Poco/Thread.h>
#include <Poco/Runnable.h>
#include <Poco/SharedPtr.h>
#include <tbb/concurrent_queue.h>
#include <tbb/pipeline.h>
#include <vector>

using Poco::Thread;
using Poco::Runnable;
using Poco::SharedPtr;


class Worker {
public:
    virtual void notifyTaskComplete() = 0;
};


/**
 * ProcessTask
 */

struct CellsTask {
	int outQueueId;
	bool isStopTask;
};

struct ProcessTask : public CellsTask {
	int number;
	int result;
};

typedef tbb::concurrent_bounded_queue<ProcessTask> ProcessTaskQueue;

/**
 * CellsInput
 */

class CellsInput : public tbb::filter {
public:
	CellsInput(ProcessTaskQueue& inQueue, int parallelCap);
	virtual ~CellsInput();

	void* operator()(void*);

private:
	int cursor_;
	int parallelCap_;
	std::vector<ProcessTask> taskBuf_;
	ProcessTaskQueue& inQueue_;
};

/**
 * CellsOutput
 */

class CellsOutput : public tbb::filter {
public:
	CellsOutput(std::vector<SharedPtr<Worker> >& workers, std::vector<ProcessTaskQueue>& outQueues);

	void* operator()(void* item);
private:
	std::vector<SharedPtr<Worker> >& workers_;
	std::vector<ProcessTaskQueue>& outQueues_;
};


/**
 * CellsProcess
 */

class CellsProcess : public tbb::filter {
public:
	CellsProcess();

	void* operator()(void* item);
private:
};


class Processor : public Runnable {
public:
	Processor(std::vector<SharedPtr<Worker> >& workers, int numOutQueues, int parallelCap);

	ProcessTaskQueue& getInQueue();
	ProcessTaskQueue& getOutQueue(int queueId);

	void start();
	void stop();

	void run();

private:
	int parallelCap_;
	ProcessTaskQueue inQueue_;
	std::vector<ProcessTaskQueue> outQueues_;
	std::vector<SharedPtr<Worker> >& workers_;

	Thread thisThread_;
};


#endif
