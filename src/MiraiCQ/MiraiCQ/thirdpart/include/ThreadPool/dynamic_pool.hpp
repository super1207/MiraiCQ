//
// Created by ganler-Mac on 2019/7/20.
//
#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <future>
#include <stack>
#include <condition_variable>
#include <exception>
#include <unordered_map>
#include <mutex>
#include <list>
#include <queue>
#include <thread>
#include <algorithm>
#include <limits>

#include <ThreadPool/util.hpp>

namespace thread_pool
{

class dynamic_pool final
{
public:
    dynamic_pool(std::size_t = 2 + std::thread::hardware_concurrency(), std::size_t = no_input);
    ~dynamic_pool();
    template <std::size_t Sz, typename Func, typename ... Args>
    auto enqueue(Func&& f, Args&& ... args) -> std::future<typename std::result_of<Func(Args...)>::type>;
    template <typename Func, typename ... Args>
    auto enqueue(Func&& f, Args&& ... args) -> std::future<typename std::result_of<Func(Args...)>::type>;
#ifdef GANLER_DEBUG
    void unsafe_view()     noexcept {
        std::cout << "----------------------------------------\n"
                  << "Queue size:" << m_task_queue.size() << '\n'
                  << "Thread size:" << m_workers.size()   << '\n'
                  << "Idle threads:" << m_idle_num << '\n';
    }
#endif
private:
    static constexpr std::size_t no_input        = std::numeric_limits<std::size_t>::max();
    using                        task_type       = std::function<void()>;
    using                        thread_index    = std::thread::id;
    using                        thread_map      = std::unordered_map<thread_index, std::thread>;

    const std::size_t            m_max_size;
    const std::size_t            m_max_idle_size;
    std::atomic<int>             m_idle_num      { 0 };
    std::mutex                   m_queue_mu;
    std::mutex                   m_map_mu;
    std::condition_variable      m_cv;
    thread_map                   m_workers;
    std::queue<task_type>        m_task_queue;
    bool                         m_shutdown      { false };

    // Helper
    void make_worker();                          // Called by locked function.
    void destroy_worker(thread_index);           // Called by locked function.
};


// Implementation
    // Init :)
inline dynamic_pool::dynamic_pool(std::size_t max_sz, std::size_t max_idle_sz)
: m_max_size(max_sz), m_max_idle_size( max_idle_sz == no_input ? max_sz / 2 : max_idle_sz)
{
    if(m_max_idle_size > m_max_size || m_max_size == 0)
        throw std::logic_error("Please make sure: max_idle_sz <= max_size && max_sz > 0\n");
}

inline void dynamic_pool::destroy_worker(thread_index index)
{
    m_task_queue.emplace([this, index](){
        if(!m_shutdown)
        {
            std::lock_guard<std::mutex> lk(m_map_mu);
            m_workers[index].join();
            m_workers.erase(index);
        }
    });
}

inline void dynamic_pool::make_worker()
{
    auto thread = std::thread{[this](){
        while(true)
        {
            std::unique_lock<std::mutex> lock(m_queue_mu);
            m_cv.wait(lock, [this](){ return m_shutdown or !m_task_queue.empty() or m_idle_num > m_max_idle_size; });
            if(m_shutdown and m_task_queue.empty()) { return; } // Game over.
            if(m_idle_num > m_max_idle_size and !m_shutdown)
            {// Cut the idle threads.
                destroy_worker(std::this_thread::get_id());
                --m_idle_num;
                return;
            }
            --m_idle_num;
            auto task = std::move(m_task_queue.front());
            m_task_queue.pop();
            lock.unlock(); // LOCK IS OVER.
            task();
            ++m_idle_num;  // ATOMIC
        }}};
    ++m_idle_num;
    m_workers[thread.get_id()] = std::move(thread);
}

inline dynamic_pool::~dynamic_pool()
{
    m_shutdown = true;
    m_cv.notify_all();
    std::for_each(m_workers.begin(), m_workers.end(), [](thread_map::value_type& x){ (x.second).join(); });
}

template <std::size_t Sz, typename Func, typename ... Args>
auto dynamic_pool::enqueue(Func &&f, Args &&... args) -> std::future<typename std::result_of<Func(Args...)>::type>
{
    using return_type = typename std::result_of<Func(Args...)>::type;
    if(m_idle_num == 0)
        for (int i = 0; i < std::min(Sz, m_max_idle_size); ++i)
            make_worker();
    MAKE_TASK()
}

template <typename Func, typename ... Args>
auto dynamic_pool::enqueue(Func &&f, Args &&... args) -> std::future<typename std::result_of<Func(Args...)>::type>
{
    using return_type = typename std::result_of<Func(Args...)>::type;
    MAKE_TASK(
            auto sz = std::min({m_task_queue.size(), m_max_idle_size, m_max_size - m_workers.size()});
            if(m_idle_num == 0 and m_workers.size() < m_max_size)
                     for (int i = 0; i < sz; ++i)
                             make_worker();)
}

}