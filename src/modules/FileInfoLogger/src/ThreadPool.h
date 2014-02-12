//
// -*- Mode: c++; tab-width: 4; -*-
// -*- ex: ts=4 -*-
//

//
// ThreadPool.h (Jakob Progsch, edited by Vadym Drozd)
// src/FileInfoLogger/ThreadPool.h
//

//
// A Thread Pool with C++11
//

//
//  Copyright(c) 2012 Jakob Progsch
//
//  This software is provided 'as-is', without any express or implied
//  warranty.In no event will the authors be held liable for any damages
//  arising from the use of this software.
//
//  Permission is granted to anyone to use this software for any purpose,
//  including commercial applications, and to alter it and redistribute it
//  freely, subject to the following restrictions :
//
//  1. The origin of this software must not be misrepresented; you must not
//  claim that you wrote the original software.If you use this software
//  in a product, an acknowledgment in the product documentation would be
//  appreciated but is not required.
//
//  2. Altered source versions must be plainly marked as such, and must not be
//  misrepresented as being the original software.
//
//  3. This notice may not be removed or altered from any source
//  distribution.
//

///////////////////////////////////////////////////////////////////////////////
// %% BeginSection: includes
//

#pragma once


#include <vector>
#include <list>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>

///////////////////////////////////////////////////////////////////////////////
// %% BeginSection: declaration
//

typedef std::function<void()> fn_type;

class ThreadPool {
public:
    ThreadPool(size_t = std::thread::hardware_concurrency());
    ~ThreadPool();

    template<class F, class... Args>
    auto addTask(F&& f, Args&&... args)
        -> std::future<typename std::result_of<F(Args...)>::type>;

    // wait for the completion of all tasks
    // (meaning that all worker are waiting for new tasks)
    void wait() const;

    void clearTaskQueue();

private:
    // need to keep track of threads so we can join them
    std::vector<std::thread> workers;
    
    // the task queue
    std::list<fn_type> tasks;

    // synchronization
    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop;

    // waiting for completion
    size_t active_worker;
    mutable std::mutex work_done_mutex;
    mutable std::condition_variable work_done_condition;

    void thread_fn();
};

///////////////////////////////////////////////////////////////////////////////
// %% BeginSection: definition
//

// the constructor just launches some amount of workers
inline ThreadPool::ThreadPool(size_t threads)
    : stop(false)
    , active_worker(threads)
{
    for (size_t i = 0; i < threads; ++i) {
        workers.emplace_back(&ThreadPool::thread_fn, this);
    }
}

inline void ThreadPool::thread_fn()
{
    for (;;) {
        std::unique_lock<std::mutex> lock(queue_mutex);

        --active_worker;
        
        while (!stop && tasks.empty()) {
            work_done_condition.notify_one(); // signal that this thread is done
            condition.wait(lock);             // and wait for more tasks
        }

        if (stop && tasks.empty())
            return;

        ++active_worker;

        //Get task from queue
        fn_type task(tasks.front());
        tasks.pop_front();
        lock.unlock();

        //And do it
        task();
    }
}

// add new work item to the pool
template<class F, class... Args>
auto ThreadPool::addTask(F&& f, Args&&... args)
    -> std::future<typename std::result_of<F(Args...)>::type>
{
    using return_type = typename std::result_of<F(Args...)>::type;
    using packaged_task_type = typename std::packaged_task<return_type()>;

    // don't allow addTask after stopping the pool
    if (stop) {
        throw std::runtime_error("enqueue on stopped ThreadPool");
        //NOTREACHED
    }

    //Prepare and add task to queue
    auto binded_fn = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
    auto task = std::make_shared<packaged_task_type>(binded_fn);
    std::future<return_type> res = task->get_future();

    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        tasks.emplace_back([task](){ (*task)(); });
    }

    //Notify that there is new task in queue
    condition.notify_one();
    return res;
}

inline void ThreadPool::wait() const
{
    std::unique_lock<std::mutex> lock(work_done_mutex);

    // wait until all threads are done and tasks are empty
    while (!(active_worker == 0 && tasks.empty()))
        work_done_condition.wait(lock);
}

inline void ThreadPool::clearTaskQueue() 
{
    std::unique_lock<std::mutex> lock(this->queue_mutex);
    tasks.clear();
}
    
// the destructor joins all threads
inline ThreadPool::~ThreadPool()
{
    {
        std::unique_lock<std::mutex> lock(this->queue_mutex);
        stop = true;
    }

    condition.notify_all();
    for (size_t i = 0; i < workers.size(); ++i)
        workers[i].join();
}

//
//
//
