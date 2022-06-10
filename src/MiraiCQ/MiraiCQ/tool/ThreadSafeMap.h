#pragma once

#include <map>
#include <shared_mutex>
#include <string>

template<typename T>
class ThreadSafeMap {
public:
	void put(const std::string & k,const T & v) {
		std::unique_lock<std::shared_mutex> lock(mx_m);
		m[k] = v;
	}
	T get(const std::string& k) {
		std::shared_lock<std::shared_mutex> lock(mx_m);
		return m.at(k);
	}
	size_t size() {
		std::shared_lock<std::shared_mutex> lock(mx_m);
		return m.size();
	}
	size_t remove(const std::string& k) {
		std::unique_lock<std::shared_mutex> lock(mx_m);
		return m.erase(k);
	}

private:
	std::shared_mutex mx_m;
	std::map<std::string,T> m;
};
