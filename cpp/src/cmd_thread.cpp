#include <thread>
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <queue>
#include <vector>
#include <mutex>

#include "pipelineRunner.hpp"


class ThreadSafeCmdProcessor {
    public:
        ThreadSafeCmdProcessor() : thread_running(false) {}

        void start_thread() {
            thread_running = true;
            t(thread_func);
        }

        void stop_thread() {
            running_mutex.lock();
            thread_running = false;
            running_mutex.unlock();

            t.join();
        }

        void push_task(CommandPipeline &task) {
            tasks_mutex.lock();
            tasks.push(task);
            tasks_mutex.unlock();
        }

        bool isResultAvailable() {
            results_mutex.lock();
            size_t size = results.size();
            results_mutex.unlock();

            return size > 0;
        }

        Result pop_result() {
            results_mutex.lock();

            Result out = results.front();
            results.pop();

            results_mutex.unlock();

            return out;

        }

    private:
        std::thread t;
        std::queue<CommandPipeline> tasks;
        std::queue<Result> results;
        std::mutex tasks_mutex;
        std::mutex results_mutex;
        std::mutex running_mutex;
        bool thread_running;

        void thread_func() {

            for (;;) {

                running_mutex.lock();
                if (thread_running == true) break;
                running_mutex.unlock();

                if (tasks.size() > 0) {
                    std::vector<CommandPipeline> task = pop_task();
                    Result res = runPipeline(task);
                    push_result(res);
                }

            }
        }

        std::vector<CommandPipeline> pop_task() {
            tasks_mutex.lock();

            std::vector<CommandPipeline> out = tasks.front();
            tasks.pop();

            tasks_mutex.unlock();

            return out;
        }

        void push_result(Result result) {
            results_mutex.lock();

            results.push(result);

            results_mutex.unlock();
        }
};
