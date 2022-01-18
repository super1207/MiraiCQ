#pragma once
#ifndef _SMART_TP_H
#define _SMART_TP_H

#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <utility>
#include <vector>
#include <iostream>
#include <smtTP/MutexQueue.h>

using namespace std;


class SmartTP{
private:
	enum class ThreadStates {waiting = 1, running = 2, stopped = 3};
	enum class ThreadTypes {core = 1, extra = 2};

	//using ThreadPtr = shared_ptr<thread>;
	//using ThreadId = atomic<int>;

	/*
	struct SmartThread{

		thread* _t;
		ThreadId _id;
		ThreadState _state;
		ThreadType _type;
	
		SmartThread(){
			_t = nullptr;
			_id = 0;
			_state.store(ThreadStates::waiting);
		}
	
	};*/

	//using SmartThreadPtr = shared_ptr<SmartThread>;

	class Worker{
	private:
		int worker_id;

		SmartTP* m_tp; //the thread pool it belongs to.
	public:
		Worker(SmartTP* tp, const int id) : m_tp(tp), worker_id(id){}

		void operator()(){
			function<void()> func;
			bool dequeued;
			while(!m_tp->m_isClosed){
				{
					unique_lock<mutex> lock(m_tp->m_cond_mutex);
					if(m_tp->m_queue.isEmpty()){
						m_tp->m_cond_lock.wait(lock);
					}
					dequeued = m_tp->m_queue.dequeue(func);
				}
				if(dequeued){
					m_tp->m_states[worker_id].store(ThreadStates::running);
					func();
					m_tp->checkSysStatus();
				}
			}
		}
	};

	bool m_isClosed;

	MutexQueue<function<void()>> m_queue;

	vector<thread> m_threads;

	vector<atomic<ThreadTypes>> m_types;

	vector<atomic<ThreadStates>> m_states;

	int m_current_thread_no;

	mutex m_cond_mutex, m_sys_check_mutex;

	condition_variable m_cond_lock;
	
public:

	SmartTP(const int default_thread_n, const int max_thread_n) : m_threads(vector<thread>(max_thread_n)),
								m_states(vector<atomic<ThreadStates>>(max_thread_n)), 
								m_types(vector<atomic<ThreadTypes>>(max_thread_n)), 
								m_current_thread_no(default_thread_n), 
								m_isClosed(false){}
	
	SmartTP(const SmartTP &) = delete;
	
	SmartTP(const SmartTP &&) = delete;

	SmartTP & operator=(const SmartTP &) = delete;
	SmartTP & operator=(SmartTP &&) = delete;

	void init();
	void close();
	void checkSysStatus();

	template<typename F, typename...Args>
	auto submit(F&& f, Args&&... args) -> future<decltype(f(args...))>{
	
		function<decltype(f(args...))()> tmpFunc = bind(forward<F>(f), forward<Args>(args)...);

		auto task_ptr = make_shared<packaged_task<decltype(f(args...))()>>(tmpFunc);

		function<void()> wrapper_func = [task_ptr](){
			(*task_ptr)();
		};

		m_queue.enqueue(wrapper_func);

		m_cond_lock.notify_one();

		return task_ptr->get_future();
	}
};

/**
 *	A number n of threads is created in the thread pool, 
 *	waiting to be called on execution.
 */
void SmartTP::init(){
	for(int i = 0; i < m_current_thread_no; ++i){
		m_threads[i] = thread(Worker(this, i));
		m_types[i].store(ThreadTypes::core);
		m_states[i].store(ThreadStates::waiting);
	}
}
/**
 *	Close the whole thread pool upon release all resources.
 */	
void SmartTP::close(){
	while(m_queue.size() > 0);
	m_isClosed = true;
	m_cond_lock.notify_all();
	for(size_t i = 0; i < m_threads.size(); ++i){
		if(m_threads[i].joinable()){
			m_threads[i].join();		
		}
	}
	//cout << static_cast<int>(m_current_thread_no) << endl;
}

void SmartTP::checkSysStatus(){
	unique_lock<mutex> lock(m_sys_check_mutex);
	bool tmp = true;
	if(m_current_thread_no >= (int)m_threads.size()) return;
	//cout << "checking sys status" << endl;
	for(int i = 0; i < m_current_thread_no; ++i){
		ThreadStates curState = m_states[i].load();
		//cout << static_cast<int>(curState) << endl;
		if(curState == ThreadStates::waiting){
			tmp = false;
			break;
		}
	}
	if(tmp){
		m_threads[m_current_thread_no] = thread(Worker(this, m_current_thread_no));
		m_states[m_current_thread_no].store(ThreadStates::waiting);
		m_current_thread_no++;
	}
}

#endif
