#ifndef __THREAD_POOL_HPP
#define __THREAD_POOL_HPP

// #include <iostream>

#include <functional>
#include <future>
#include <queue>

class ThreadPool {
    void Worker(void) {
        while (true) {
            std::function<void()> task;

            {
                std::unique_lock<std::mutex> lock(mtx_);
                cv_.wait(lock, [this]() { return this->stop_ || !this->my_queue_.empty(); });

                if (stop_ && my_queue_.empty())
                    return;

                // 走到这一步表命一定还有待处理的任务, 但线程池可能停止可能没停止
                task = std::move(my_queue_.front());
                my_queue_.pop();
            }

            task();
        }
    }

    ThreadPool(int thread_num = 2) : stop_(false) {
        for (int i = 0; i < thread_num; ++i) {
            // workers_.emplace_back([this]() {this->Worker(); });
            std::thread t(&ThreadPool::Worker, this);
            // workers_.emplace_back(&ThreadPool::Worker, this);
            workers_.push_back(std::move(t));
        }
    }

public:
    static ThreadPool& Instance() {
        static ThreadPool instance{};
        return instance;
    }

    ~ThreadPool() {
        stop_.store(true);

        cv_.notify_all();

        for (auto& t : workers_) {
            if (t.joinable()) {
                // std::cout << "线程：" << t.get_id() << "加入" << std::endl;
                t.join();
            }
        }
    }

    template <class F, class... Args>
    auto Enques(F&& f, Args&&... arg) -> std::future<std::invoke_result_t<F, Args...>> {
        using ReturnType = std::invoke_result_t<F, Args...>;

        auto task_p = std::make_shared<std::packaged_task<ReturnType()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(arg)...));
        std::future<ReturnType> return_future = task_p->get_future();

        {
            std::lock_guard<std::mutex> lock(mtx_);
            if (stop_) {
                throw std::runtime_error("线程池停止");
            }

            my_queue_.emplace([task_p]() { (*task_p)(); });
        }

        cv_.notify_one();

        return return_future;
    }

private:
    std::atomic_bool stop_;
    std::condition_variable cv_;
    std::mutex mtx_;
    std::vector<std::thread> workers_;
    std::queue<std::function<void()>> my_queue_;
};

#endif  // !__THREAD_POOL_HPP
