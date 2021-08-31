#include "InputTool.h"

#include <iostream>

#include "StrTool.h"

using namespace std;

std::vector<std::string> InputTool::get_line() 
{
	string line;
	getline(cin, line);
	std::vector<std::string> dat_vec;
	auto ret_vec = StrTool::match_all(line, "[\\S]+");
	for (auto& i : ret_vec)
	{
		dat_vec.push_back(i.at(0));
	}
	return dat_vec;
}

InputTool::InputTool()
{
}

InputTool::~InputTool()
{
}