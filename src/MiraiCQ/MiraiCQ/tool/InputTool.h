#pragma once

#include <vector>
#include <string>

class InputTool
{
public:
	static std::vector<std::string> get_line() noexcept;
private:
	InputTool();
	~InputTool();

};

