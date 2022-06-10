#pragma once

#include <functional>

class AutoDoSth {
public:
	AutoDoSth(std::function<void(void)> fun);
	~AutoDoSth();
private:
	std::function<void(void)> fun;
};
