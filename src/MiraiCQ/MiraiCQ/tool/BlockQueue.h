#pragma once

#include <list>
#include <mutex>
#include <condition_variable>
#include <chrono>

template <typename T>
class BlockQueue {
public:
	void push(const T& t) {
		std::lock_guard<std::mutex> lock(mx_l);
		l.push_back(t);
		cv.notify_one();
	}
	T pop(int timeout) {
		std::unique_lock<std::mutex> lock(mx_l);
		bool wait_ret = cv.wait_for(lock, std::chrono::milliseconds(timeout), [&] {return l.size() > 0; });
		if (!wait_ret) {
			//timeout
			throw std::runtime_error("wait for pop timeout");
		}
		auto b = (*l.begin());
		l.pop_front();
		return b;
	}
private:
	std::mutex mx_l;
	std::list<T> l;
	std::condition_variable cv;
};