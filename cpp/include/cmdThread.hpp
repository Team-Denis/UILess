#pragma once

#include <thread>
#include <queue>
#include <vector>
#include <mutex>
#include <condition_variable>

#include "pipelineRunner.hpp"
#include "result.hpp"

class ThreadSafeCmdProcessor {
public:
    ThreadSafeCmdProcessor();
    ~ThreadSafeCmdProcessor();

    // Delete copy constructor and copy assignment to prevent copying
    ThreadSafeCmdProcessor(const ThreadSafeCmdProcessor&) = delete;
    ThreadSafeCmdProcessor& operator=(const ThreadSafeCmdProcessor&) = delete;

    // Public interface
    void startThread();
    void stopThread();
    void pushTask(CommandPipeline task);
    bool isResultAvailable();
    std::vector<Result> popResults();

private:
    std::thread                 worker_thread;
    std::queue<CommandPipeline> tasks_queue;
    std::queue<Result>          results_queue;

    std::mutex tasks_mutex;
    std::mutex results_mutex;
    std::mutex running_mutex;

    std::condition_variable tasks_cv;
    std::condition_variable results_cv;

    bool thread_running;

    // Worker thread function
    void thread_func();

    // Helper functions
    bool popTask       (CommandPipeline& task);
    void pushResults   (const std::vector<Result>& results);
};