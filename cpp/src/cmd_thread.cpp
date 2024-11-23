#include "cmd_thread.hpp"

ThreadSafeCmdProcessor::ThreadSafeCmdProcessor() : thread_running(false) {}


void ThreadSafeCmdProcessor::start_thread() {
    thread_running = true;
    t = std::thread(&ThreadSafeCmdProcessor::thread_func, this);
}

void ThreadSafeCmdProcessor::stop_thread() {
    running_mutex.lock();
    thread_running = false;
    running_mutex.unlock();

    if (t.joinable()) {
        t.join();
    }
}

void ThreadSafeCmdProcessor::push_task(CommandPipeline task) {
    std::lock_guard<std::mutex> lock(tasks_mutex);
    tasks.push(task);
}

bool ThreadSafeCmdProcessor::isResultAvailable() {
    std::lock_guard<std::mutex> lock(results_mutex);
    return !results.empty();
}

Result ThreadSafeCmdProcessor::pop_result() {
    std::lock_guard<std::mutex> lock(results_mutex);
    Result out = results.front();
    results.pop();
    return out;
}

void ThreadSafeCmdProcessor::thread_func() {
    for (;;) {
        running_mutex.lock();
        if (!thread_running) {
            running_mutex.unlock();
            break;
        }
        running_mutex.unlock();

        if (!tasks.empty()) {
            CommandPipeline task = pop_task();
            
            std::vector<Result> res = runPipeline(task);
            push_result(res);
        }
    }
}

CommandPipeline ThreadSafeCmdProcessor::pop_task() {
    std::lock_guard<std::mutex> lock(tasks_mutex);
    CommandPipeline out = tasks.front();
    tasks.pop();
    return out;
}

void ThreadSafeCmdProcessor::push_result(std::vector<Result> results) {
    std::lock_guard<std::mutex> lock(results_mutex);
    for (const auto& result : results) {
        this->results.push(result);
    }
}