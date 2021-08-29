#include "InputTool.h"

#include <iostream>
#include <regex>

using namespace std;

std::vector<std::string> InputTool::get_line() noexcept
{
	string line;
	getline(cin, line);
	regex r("[\\S]+");
	smatch sm;
	string::const_iterator searchStart(line.cbegin());
	std::vector<std::string> dat_vec;
	while (regex_search(searchStart, line.cend(), sm, r))
	{
		dat_vec.push_back(sm[0].str());
		searchStart = sm.suffix().first;
	}
	return dat_vec;
}

InputTool::InputTool()
{
}

InputTool::~InputTool()
{
}