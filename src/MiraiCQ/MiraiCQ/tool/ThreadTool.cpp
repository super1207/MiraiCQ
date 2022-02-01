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
		MiraiLog::get_instance()->add_fatal_log("ThreadTool","提交新任务失败，累积任务数量过多");
		return false;
	}
	// 没有空余线程，则新增一个线程，保证任务能顺利进行
	if (unused_thread_nums == 0)
	{
		add_new_thread();
	}
	// 将任务放入任务队列
	{
		std::lock_guard<std::mutex> lock(mx_task_list);
		task_list.push_back(task);
		cv.notify_one();
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
	std::lock_guard<std::mutex> lock(mx_task_list);
	return task_list.size();
}

ThreadTool::ThreadTool()
{
	// 增加一个守护线程
	++cur_thread_nums;
	std::thread([&]() {
		int i = 0;
		while (true) {
			// 没有获取到任务，睡眠一会儿
			TimeTool::sleep(200);
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
		MiraiLog::get_instance()->add_warning_log("ThreadTool", "已有线程过多，增加新线程失败");
		return;
	}
	++cur_thread_nums;
	std::thread([this]() {
		while (true)
		{
			std::function<void()> task = nullptr;
			{
				++unused_thread_nums;
				/* 等待一个任务 */
				std::unique_lock<std::mutex> lock(mx_task_list);
				bool is_get = cv.wait_for(lock, std::chrono::seconds(5), [this]() {
					return task_list.size() > 0;
				});
				--unused_thread_nums;
				if (is_get) {
					/* 成功等到一个任务 */
					task = (*task_list.begin());
					task_list.pop_front();
				}
				else {
					/* 没有成功等到一个任务，则结束线程 */
					break;
				}
			}
			/* 执行任务 */
			try {
				task();
			}
			catch (const std::exception& e) {
				MiraiLog::get_instance()->add_fatal_log("ThreadTool", std::string("在ThreadTool中发生未知错误:") + e.what());
				exit(-1);
			}
			
		}
		--cur_thread_nums;
	}).detach();
}
