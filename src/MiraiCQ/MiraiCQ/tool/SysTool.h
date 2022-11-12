#pragma once
class SysTool
{
public:
	static double get_cpu_usage(int pid);
	static double get_mem_usage(int pid);
};

