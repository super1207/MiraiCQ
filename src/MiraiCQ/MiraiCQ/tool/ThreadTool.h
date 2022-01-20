#pragma once
#include <functional>
#include <list>
#include <atomic>
#include <mutex>

class ThreadTool
{
public:
	/*
	* 描述：提交一个任务
	* 参数`task`：要提交的任务
	*/
	void submit(const std::function<void()> & task);
	ThreadTool();
	~ThreadTool();
private:
	void add_new_thread();
	std::atomic_bool is_stop = false;
	/* 记录当前存在的线程数量 */
	std::atomic_int cur_thread_nums = 0;
	/* 记录当前空余线程 */
	std::atomic_int unused_thread_nums = 0;
	std::mutex mx_task_list;
	std::list<std::function<void()>> task_list;
};

