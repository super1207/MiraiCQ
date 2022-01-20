#include "ThreadTool.h"
#include "TimeTool.h"
#include <thread>
#include "../log/MiraiLog.h"
#include <spdlog/fmt/fmt.h>



bool ThreadTool::submit(const std::function<void()> & task)
{
	/* 当前任务过多，拒绝提交新任务 */
	if (get_task_list_nums() > max_task_nums)
	{
		MiraiLog::get_instance()->add_debug_log("ThreadTool","提交新任务失败，累积任务数量过多");
		return false;
	}
	// 将任务放入任务队列
	{
		std::unique_lock<std::shared_mutex> lock(mx_task_list);
		task_list.push_back(task);
	}
	// 没有空余线程，则新增一个线程，保证任务能顺利进行
	if (unused_thread_nums == 0)
	{
		add_new_thread();
	}
	return true;
}

ThreadTool* ThreadTool::get_instance()
{
	static ThreadTool t;
	return &t;
}

int ThreadTool::get_cur_thread_nums() const
{
	return cur_thread_nums;
}

int ThreadTool::get_unused_thread_nums() const
{
	return unused_thread_nums;
}

size_t ThreadTool::get_task_list_nums()
{
	std::shared_lock<std::shared_mutex> lock(mx_task_list);
	return task_list.size();
}

ThreadTool::ThreadTool()
{
	// 增加一个守护线程
	++cur_thread_nums;
	std::thread([&]() {
		int i = 0;
		while (true) {
			++i;
			if (i == 300)
			{
				MiraiLog::get_instance()->add_debug_log("ThreadTool", fmt::format("cur_thread_nums:{}", cur_thread_nums));
				MiraiLog::get_instance()->add_debug_log("ThreadTool", fmt::format("unused_thread_nums:{}", unused_thread_nums));
				MiraiLog::get_instance()->add_debug_log("ThreadTool", fmt::format("task_nums:{}", get_task_list_nums()));
				i = 0;
			}
			// 没有获取到任务，睡眠一会儿
			TimeTool::sleep(100);
			// 如果没有未使用的线程,但是有任务，则增加一个线程
			if (unused_thread_nums == 0 && (get_task_list_nums() > 0))
			{
				add_new_thread();
			}
		}
		// 线程退出了，实际上不会执行到这里
		// --cur_thread_nums;
	}).detach();
}


void ThreadTool::add_new_thread()
{
	// 如果当前线程数量过多，则拒绝增加新的线程
	if (cur_thread_nums > max_thread_nums)
	{
		return;
	}
	++cur_thread_nums;
	std::thread([&]() {
		// 用于标记是否空闲
		bool is_unused = false;
		while (true)
		{
			std::function<void()> task = nullptr;
			{
				// 从任务队列拿一个任务
				std::unique_lock<std::shared_mutex> lock(mx_task_list);
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
				}
				task();
			}
			else {
				// 如果没拿到，说明任务队列为空，
				if (is_unused){
					// 如果已经被标记为空闲，说明两次没拿到task，则退出
					break;
				}
				TimeTool::sleep(100);
				// 将此线程标记为空闲,然后继续循环
				++unused_thread_nums;
				is_unused = true;
			}

		}
		// 删除空闲标记
		if (is_unused) {
			--unused_thread_nums;
		}
		// 线程退出了
		--cur_thread_nums;
	}).detach();
}
