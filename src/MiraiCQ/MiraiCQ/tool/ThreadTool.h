#pragma once
#include <functional>
#include <list>
#include <atomic>
#include <shared_mutex>

class ThreadTool
{
public:
	/*
	* 描述：提交一个任务
	* 参数`task`：要提交的任务
	* 返回值：是否提交成功
	*/
	bool submit(const std::function<void()> & task);
	/*
	* 描述：获得线程池(单例模式)
	* 返回值：ThreadTool非空指针
	*/
	static ThreadTool* get_instance();
	/*
	* 描述：获得当前线程池线程数量
	* 返回值：当前线程池线程数量
	*/
	int get_cur_thread_nums() const;
	/*
	* 描述：获得当前线程池空闲线程数量
	* 返回值：当前线程池空闲线程数量
	*/
	int get_unused_thread_nums() const;
	/*
	* 描述：获得当前任务队列任务数量
	* 返回值：当前任务队列任务数量
	*/
	size_t get_task_list_nums();
private:
	ThreadTool();
	/* 新增一个线程 */
	void add_new_thread();
	/* 记录当前存在的线程数量 */
	std::atomic_int cur_thread_nums = 0;
	/* 记录当前空余线程 */
	std::atomic_int unused_thread_nums = 0;
	/* 任务队列 */
	std::shared_mutex mx_task_list;
	std::list<std::function<void()>> task_list;
};

