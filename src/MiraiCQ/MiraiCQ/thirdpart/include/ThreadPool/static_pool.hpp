#pragma once

#include <algorithm>
#include <array>
#include <atomic>
#include <condition_variable>
#include <cstddef>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

#include <ThreadPool/util.hpp>

namespace thread_pool {

class static_pool
final { // Simple thread-safe & container-free thread pool.
public:
    const std::size_t size{0};

    explicit static_pool(
            std::size_t /* suggested size */ = std::thread::hardware_concurrency() + 2);

    ~static_pool();

    template<typename Func, typename... Args>
    auto enqueue(Func &&f, Args &&... args) /* For Cpp14+ -> decltype(auto). */
    -> std::future<typename std::result_of<Func(Args...)>::type>;

    //    template <typename Func, typename ... Args>
    //    auto enqueue_advance(Func &&f, Args &&... args) /* For Cpp14+ ->
    //    decltype(auto). */
    //    -> std::future<typename std::result_of<Func(Args...)>::type>;
    void wait();

private:
    using task_type = std::function<void()>;
    // Use xx::function<> wrapper is not zero overhead.(See the link below)
    // https://www.boost.org/doc/libs/1_45_0/doc/html/function/misc.html#id1285061
    // https://www.boost.org/doc/libs/1_45_0/doc/html/function/faq.html#id1284915
    struct pool_src {
        std::mutex queue_mu;
        std::mutex wait_mu;
        std::condition_variable cv;
        std::queue<task_type> queue;
        std::atomic<int> to_finish{0};
        std::condition_variable wait_cv;
        bool shutdown{false};
    };
    std::shared_ptr<pool_src> m_shared_src;
};


// Implementation:
template<typename Func, typename... Args>
auto static_pool::enqueue(Func &&f, Args &&... args)
-> std::future<typename std::result_of<Func(Args...)>::type> {
    using return_type = typename std::result_of<Func(Args...)>::type;
    std::packaged_task<return_type()> *task = nullptr;
    try_allocate(task, std::forward<Func>(f), std::forward<Args>(args)...);
    auto result = task->get_future();
    {
        // Critical region.
        std::lock_guard<std::mutex> lock(m_shared_src->queue_mu);
        m_shared_src->queue.push([task]() {
            (*task)();
            delete task;
        });
    }
    m_shared_src->to_finish.fetch_add(1);
    m_shared_src->cv.notify_one();
    return result;
}

static_pool::static_pool(std::size_t sz)
        : m_shared_src(std::make_shared<pool_src>()), size(sz) {
    for (int i = 0; i < sz; ++i) {
        std::thread(
                [this](std::shared_ptr<pool_src> ptr) {
                    while (true) {
                        std::function<void()> task;
                        // >>> Critical region => Begin
                        {
                            std::unique_lock<std::mutex> lock(ptr->queue_mu);
                            ptr->cv.wait(
                                    lock, [&] { return ptr->shutdown or !ptr->queue.empty(); });
                            if (ptr->shutdown and ptr->queue.empty())
                                return; // Conditions to let the thread go.
                            task = std::move(ptr->queue.front());
                            ptr->queue.pop();
                        }
                        // >>> Critical region => End
                        task();
                        if (ptr->to_finish.fetch_add(-1) == 1)
                            ptr->wait_cv.notify_one();
                    }
                },
                m_shared_src)
                .detach();
    }
}

void static_pool::wait() {
    std::unique_lock<std::mutex> lock(m_shared_src->wait_mu);
    m_shared_src->wait_cv.wait(
            lock, [this] { return m_shared_src->to_finish.load() == 0; });
}

static_pool::~static_pool() {
    m_shared_src->shutdown = true;
    std::atomic_signal_fence(std::memory_order_seq_cst);
    m_shared_src->cv.notify_all();
}

} // namespace thread_pool