#include "TimeTool.h"

#include "Windows.h"

void TimeTool::sleep(unsigned long ms) 
{
	if (ms == 0)
	{
		Sleep(1);
	}
	else
	{
		Sleep(ms);
	}
	
}

unsigned long long TimeTool::get_tick_count() 
{
	return GetTickCount64();
}

TimeTool::TimeTool()
{
}
