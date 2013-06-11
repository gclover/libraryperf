
#include "processor.h"


/**
 * CellsInput
 */

// serial_out_of_order
CellsInput::CellsInput(ProcessTaskQueue& inQueue, int parallelCap)
: filter(filter_is_serial), cursor_(0), parallelCap_(parallelCap),
  taskBuf_(parallelCap), inQueue_(inQueue) {
}

CellsInput::~CellsInput() {
}

void* CellsInput::operator()(void*) {
	ProcessTask& nextTask = taskBuf_[cursor_];
	inQueue_.pop(nextTask);
	if (nextTask.isStopTask)
		return NULL;
	cursor_ = ++cursor_ % parallelCap_;
	return &nextTask;
}

/**
 * CellsOutput
 */

CellsOutput::CellsOutput(std::vector<SharedPtr<Worker> >& workers, std::vector<ProcessTaskQueue>& outQueues)
: filter(filter_is_serial), workers_(workers), outQueues_(outQueues) {
}

void* CellsOutput::operator()(void* item) {
	ProcessTask& task = *static_cast<ProcessTask*>(item);
	outQueues_[task.outQueueId].push(task);
	workers_[task.outQueueId]->notifyTaskComplete();
	return NULL;
}

/**
 * CellsProcess
 */

CellsProcess::CellsProcess()
: filter(parallel) {
}

void* CellsProcess::operator()(void* item) {
	ProcessTask& task = *static_cast<ProcessTask*>(item);
	task.result = task.number * 2;
	return &task;
}


/**
 * Processor
 */

Processor::Processor(std::vector<SharedPtr<Worker> >& workers, int numOutQueues, int parallelCap)
: workers_(workers), parallelCap_(parallelCap), outQueues_(numOutQueues) {
}

ProcessTaskQueue& Processor::getInQueue() {
	return inQueue_;
}

ProcessTaskQueue& Processor::getOutQueue(int queueId) {
	return outQueues_[queueId];
}

void Processor::start() {
	thisThread_.start(*this);
}

void Processor::stop() {
	ProcessTask task;
	task.isStopTask = true;
	inQueue_.push(task);
}

void Processor::run() {

	CellsInput input(inQueue_, parallelCap_);
	CellsOutput output(workers_, outQueues_);
	CellsProcess process;

	tbb::pipeline pipeline;
	pipeline.add_filter(input);
	pipeline.add_filter(process);
	pipeline.add_filter(output);
	pipeline.run(parallelCap_);
}



