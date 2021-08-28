#include "TimeTool.h"

#include "Windows.h"

void TimeTool::sleep(unsigned long ms) noexcept
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

unsigned long long TimeTool::get_tick_count() noexcept
{
	return GetTickCount64();
}

TimeTool::TimeTool()
{
}
