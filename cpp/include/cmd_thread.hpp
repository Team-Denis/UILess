#ifndef CMD_THREAD_HPP
#define CMD_THREAD_HPP

#include <thread>
#include <queue>
#include <vector>
#include <mutex>
#include "pipelineRunner.hpp"
#include "result.hpp"

class ThreadSafeCmdProcessor {
public:
    ThreadSafeCmdProcessor();

    void start_thread();
    void stop_thread();
    void push_task(CommandPipeline task);
    bool isResultAvailable();
    Result pop_result();

private:
    std::thread t;
    std::queue<CommandPipeline> tasks;
    std::queue<Result> results;
    std::mutex tasks_mutex;
    std::mutex results_mutex;
    std::mutex running_mutex;
    bool thread_running;

    void thread_func();
    CommandPipeline pop_task();
    void push_result(std::vector<Result> results);
};

#endif // CMD_THREAD_HPP