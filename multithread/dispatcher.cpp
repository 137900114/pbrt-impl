#include "dispatcher.h"
#include <thread>

Dispatcher::Dispatcher(uint32 nThread) {
	al_assert(nThread != 0, "Dispatcher::Dispatcher : the thread number should not be 0");
	this->nThread = nThread;
}

void Dispatcher::Dispatch(std::function<void(uint32)> func) {
	if (nThread == 1) {
		func(0);
	}
	else {
		vector<thread> threads;
		al_for(i,1,nThread) {
			threads.emplace_back(thread(func, i));
		}
		func(0);
		//wait for threads to join
		al_for(i,0,nThread-1) {
			threads[i].join();
		}
	}
}