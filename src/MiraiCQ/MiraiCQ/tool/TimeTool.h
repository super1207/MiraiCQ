#pragma once

class TimeTool
{
public:
	/*
	* 描述：睡眠一段时间
	* 参数`ms`：要睡眠的时间（单位毫秒）,若ms为0，等效于ms为1
	*/
	static void sleep(unsigned long ms)noexcept;

	/*
	* 描述：返回毫秒级时间戳
	* 返回值：毫秒级时间戳
	*/
	static unsigned long long get_tick_count()noexcept;

private:
	TimeTool();
};

