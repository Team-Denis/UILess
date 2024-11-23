#include <thread>
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <queue>
#include <vector>
#include <mutex>


class ThreadSafeCmdProcessor {
    public:
        ThreadSafeCmdProcessor() : thread_running(false) {}

        void push_task(std::vector<Command> &task) {
            tasks_mutex.lock();
            tasks.push(task);
            tasks_mutex.unlock();
        }

        Result pop_result() {
            results_mutex.lock();

            Result out = results.front();
            results.pop();

            results_mutex.unlock();

            return out;

        }

    private:
        std::queue<std::vector<Command>> tasks;
        std::queue<Result> results;
        std::mutex tasks_mutex;
        std::mutex results_mutex;
        bool thread_running;

        std::vector<Command> pop_task() {
            tasks_mutex.lock();

            std::vector<Command> out = tasks.front();
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


int main() {


    return 0;
}