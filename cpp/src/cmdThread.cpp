#include "cmdThread.hpp"

ThreadSafeCmdProcessor::ThreadSafeCmdProcessor()
    : thread_running(false) {}

ThreadSafeCmdProcessor::~ThreadSafeCmdProcessor() {
    stopThread();
}

void ThreadSafeCmdProcessor::startThread() {
    std::lock_guard lock(running_mutex);
    if (!thread_running) {
        thread_running = true;
        worker_thread = std::thread(&ThreadSafeCmdProcessor::thread_func, this);
    }
}

void ThreadSafeCmdProcessor::stopThread() {
    {
        std::lock_guard lock(running_mutex);
        if (!thread_running) {
            return; // Thread is already stopped
        }
        thread_running = false;
    }
    tasks_cv.notify_all(); // Wake up the thread if it's waiting

    if (worker_thread.joinable()) {
        worker_thread.join();
    }
}

void ThreadSafeCmdProcessor::pushTask(CommandPipeline task) {
    {
        std::lock_guard lock(tasks_mutex);
        tasks_queue.push(std::move(task));
    }
    tasks_cv.notify_one(); // Notify the worker thread that a new task is available
}

bool ThreadSafeCmdProcessor::isResultAvailable() {
    std::lock_guard lock(results_mutex);
    return !results_queue.empty();
}

std::vector<Result> ThreadSafeCmdProcessor::popResults() {
    std::lock_guard lock(results_mutex);
    std::vector<Result> available_results;

    while (!results_queue.empty()) {
        available_results.push_back(std::move(results_queue.front()));
        results_queue.pop();
    }

    return available_results;
}

void ThreadSafeCmdProcessor::thread_func() {
    while (true) {
        CommandPipeline task;
        {
            std::unique_lock lock(tasks_mutex);

            tasks_cv.wait(lock, [this] {
                std::lock_guard run_lock(running_mutex);
                return !tasks_queue.empty() || !thread_running;
            });

            // Check if we should stop the thread
            {
                std::lock_guard run_lock(running_mutex);

                if (!thread_running && tasks_queue.empty()) {
                    break;
                }
            }

            if (!tasks_queue.empty()) {
                task = std::move(tasks_queue.front());
                tasks_queue.pop();
            } else {
                continue; // Spurious wakeup or thread_running was set to false
            }
        }

        // Process the task outside the lock
        try {
            std::vector<Result> res = runPipeline(task);
            pushResults(res);
        } catch (const std::exception& e) {
            // Handle exceptions from runPipeline and push an error Result
            Result error_res;
            error_res.exit_code = -1;
            error_res.stdout_output = "";
            error_res.stderr_output = std::string("Exception: ") + e.what();
            pushResults({ error_res });
        }
    }
}

bool ThreadSafeCmdProcessor::popTask(CommandPipeline& task) {
    std::lock_guard lock(tasks_mutex);

    if (tasks_queue.empty()) {
        return false;
    }

    task = std::move(tasks_queue.front());
    tasks_queue.pop();
    return true;
}

void ThreadSafeCmdProcessor::pushResults(const std::vector<Result>& results) {
    {
        std::lock_guard lock(results_mutex);

        for (const auto& result : results) {
            results_queue.push(result);
        }
    }

    results_cv.notify_one(); // Notify the UI thread if it's waiting for results
}