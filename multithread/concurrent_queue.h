#pragma once
#include "common/common.h"
#include <thread>
#include <queue>
#include <optional>

template<typename T>
class ConcurrentQueue {
public:
	ConcurrentQueue() {}

	bool			 empty() {
		access_lock.lock();
		bool rv = queue.empty();
		access_lock.unlock();
		return rv;
	}

	uint64			 size() {
		access_lock.lock();
		uint64 rv = queue.size();
		access_lock.unlock();
		return rv;
	}

	std::optional<T> try_pop() {
		std::optional<T> rv = {};
		access_lock.lock();
		if (!queue.empty()) {
			rv = { std::move(queue.front()) };
			queue.pop();
		}
		access_lock.unlock();
		return rv;
	}

	void			 push(const T& item) {
		access_lock.lock();
		queue.push(item);
		access_lock.unlock();
	}

private:
	mutex access_lock;
	queue<T>  queue;
};