#pragma once
#include "common/common.h"
#include <functional>

class Dispatcher {
public:
	al_add_ptr_t(Dispatcher);
	Dispatcher(uint32 nThread);

	void Dispatch(std::function<void(uint32)> func);

private:
	uint32 nThread;
};