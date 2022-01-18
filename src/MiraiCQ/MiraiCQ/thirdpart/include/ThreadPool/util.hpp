//
// Created by ganler-Mac on 2019/7/23.
//
#pragma once

//#define GANLER_DEBUG
#define TASK_USE_RAW_PTR(X)  \
    std::packaged_task<return_type()>* task = nullptr;/* Why raw pointer? See comments in static_pool.hpp's enqueue().*/\
    try_allocate(task, std::forward<Func>(f), std::forward<Args>(args)...);\
    auto result = task->get_future();\
    {\
        std::lock_guard<std::mutex> lock(m_queue_mu);\
        m_task_queue.emplace([task](){ (*task)(); delete task; });\
    }\
    X;\
    m_cv.notify_one();\
    return result;

#define TASK_USE_SMART_PTR(X) \
    auto task = std::make_shared<std::packaged_task<return_type ()>>(\
        std::bind(std::forward<Func>(f), std::forward<Args>(args)...));\
    auto result = task->get_future();\
    {\
        std::lock_guard<std::mutex> lock(m_queue_mu);\
        m_task_queue.emplace([task](){ (*task)(); });\
    }\
    X\
    m_cv.notify_one();\
    return result;

#define MAKE_TASK(X) TASK_USE_RAW_PTR(X)

#ifdef GANLER_DEBUG
#include <iostream>
#define GANLER_DEBUG_DETAIL(X) { std::cout << "line: "<< __LINE__ << "\t " << (#X) << " is " << (X) << '\n'; }
#define DPOOL_UNSAFE_VIEW(X)   { X.unsafe_view(); }
#else
#define GANLER_DEBUG_DETAIL(X) {}
#define DPOOL_UNSAFE_VIEW(X)   {}
#endif

#include <type_traits>
#include <functional>

namespace thread_pool
{

template <typename Type, typename Func, typename ... Args>
inline void try_allocate(Type& task, Func&& f, Args&& ... args)
{
    try{
        task = new typename std::remove_pointer<Type>::type(std::bind(std::forward<Func>(f), std::forward<Args>(args)...));
    } catch (const std::exception& e) {
        if(task != nullptr)
            delete task;
        throw e;
    }
}

}