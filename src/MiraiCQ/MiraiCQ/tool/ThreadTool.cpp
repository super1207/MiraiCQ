#include "ThreadTool.h"
#include "TimeTool.h"
#include <thread>
#include "../log/MiraiLog.h"
#include <spdlog/fmt/fmt.h>

void ThreadTool::submit(const std::function<void()> & task)
{
	{
		// 将任务放入任务队列
		std::lock_guard<std::mutex> lock(mx_task_list);
		task_list.push_back(task);
	}
	// 没有空余线程，则新增一个线程，保证任务能顺利进行
	if (unused_thread_nums == 0)
	{
		add_new_thread();
	}
	
}

static bool has_task(const std::list<std::function<void()>>& task_list, std::mutex& mx_task_list)
{
	std::lock_guard<std::mutex> lock(mx_task_list);
	return ((task_list.size() > 0) ? true : false);
}

static size_t get_task_nums(const std::list<std::function<void()>>& task_list, std::mutex& mx_task_list)
{
	std::lock_guard<std::mutex> lock(mx_task_list);
	return task_list.size();
}

ThreadTool::ThreadTool()
{
	is_stop = false;
	// 增加一个守护线程
	++cur_thread_nums;
	//MiraiLog::get_instance()->add_debug_log("ThreadTool", fmt::format("cur_thread_nums:{}", cur_thread_nums));
	std::thread([&]() {
		int i = 0;
		while (true) {
			++i;
			if (i == 300)
			{
				MiraiLog::get_instance()->add_debug_log("ThreadTool", fmt::format("cur_thread_nums:{}", cur_thread_nums));
				MiraiLog::get_instance()->add_debug_log("ThreadTool", fmt::format("unused_thread_nums:{}", unused_thread_nums));
				MiraiLog::get_instance()->add_debug_log("ThreadTool", fmt::format("task_nums:{}", get_task_nums(task_list, mx_task_list)));
				i = 0;
			}
			TimeTool::sleep(100);
			// 如果没有未使用的线程,但是有任务，则增加一个线程
			if (unused_thread_nums == 0 && has_task(task_list,mx_task_list))
			{
				add_new_thread();
			}
			// 如果线程池已经停止，任务队列没有任务，则退出守护线程
			std::lock_guard<std::mutex> lock(mx_task_list);
			if (is_stop && !has_task(task_list, mx_task_list))
				break;
		}
		// 线程退出了
		--cur_thread_nums;
		//MiraiLog::get_instance()->add_debug_log("ThreadTool", fmt::format("cur_thread_nums:{}", cur_thread_nums));
	}).detach();
}

ThreadTool::~ThreadTool()
{
	// 停止线程池
	is_stop = false;
	// 等待线程全部停止
	while (cur_thread_nums)
		TimeTool::sleep(10);
}

void ThreadTool::add_new_thread()
{
	++cur_thread_nums;
	//MiraiLog::get_instance()->add_debug_log("ThreadTool", fmt::format("cur_thread_nums:{}", cur_thread_nums));
	std::thread([&]() {
		// 用于标记是否空闲
		bool is_unused = false;
		while (true)
		{
			std::function<void()> task = nullptr;
			{
				// 从任务队列拿一个任务
				std::lock_guard<std::mutex> lock(mx_task_list);
				if (task_list.size() > 0) {
					task = (*task_list.begin());
					task_list.pop_front();
				}
			}
			if (task) {
				// 如果拿到了，就执行
				if (is_unused)
				{
					// 删除空闲标记
					is_unused = false;
					--unused_thread_nums;
					//MiraiLog::get_instance()->add_debug_log("ThreadTool", fmt::format("unused_thread_nums:{}", unused_thread_nums));
				}
				task();
			}
			else {
				// 如果没拿到，说明任务队列为空，
				if (is_unused){
					// 如果已经被标记为空闲，说明两次没拿到task，则退出
					break;
				}
				//如果线程池停止，又任务队列为空，则退出线程
				if (is_stop && !has_task(task_list, mx_task_list)) {
						break;
				}
				TimeTool::sleep(100);
				// 将此线程标记为空闲,然后继续循环
				++unused_thread_nums;
				//MiraiLog::get_instance()->add_debug_log("ThreadTool", fmt::format("unused_thread_nums:{}", unused_thread_nums));
				is_unused = true;
			}

		}
		// 删除空闲标记
		if (is_unused) {
			--unused_thread_nums;
			//MiraiLog::get_instance()->add_debug_log("ThreadTool", fmt::format("unused_thread_nums:{}", unused_thread_nums));
		}
		// 线程退出了
		--cur_thread_nums;
		//MiraiLog::get_instance()->add_debug_log("ThreadTool", fmt::format("cur_thread_nums:{}", cur_thread_nums));
	}).detach();
}
