#ifndef POTOPOTO_BACKGROUNDTASK_H
#define POTOPOTO_BACKGROUNDTASK_H

#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <functional>
#include <optional>
#include <chrono>

enum class TaskStatus {
    SUCCESS,
    TIMEOUT,
    CANCELLED,
    ERROR
};

// Abstract class for background tasks
template<typename ResultType>
class BackgroundTask {
public:
    using TaskCallback = std::function<void(TaskStatus)>;

    // Constructor to set the timeout
    BackgroundTask(std::chrono::milliseconds timeout_duration = std::chrono::milliseconds::max())
            : is_running(false), is_cancelled(false), timeout(timeout_duration) {}

    virtual ~BackgroundTask() {
        Stop();
    }

    // Start the background task with an optional callback
    bool Run(TaskCallback callback = nullptr) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!is_running) {
            is_running = true;
            is_cancelled = false;
            error_code = TaskStatus::SUCCESS;
            on_complete = callback;  // Store the callback
            try {
                task_thread = std::thread(&BackgroundTask::ExecuteTask, this);
            } catch (...) {
                is_running = false;
                error_code = TaskStatus::ERROR;
                return false;
            }
            return true;
        }
        return false;
    }

    // Stop the background task
    void Stop() {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (is_running) {
                is_cancelled = true;
                cv.notify_all();
            }
        }

        if (task_thread.joinable()) {
            task_thread.join();
        }
    }

    // Check if the task is running
    bool IsRunning() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return is_running;
    }

    // Retrieve the result of the task with optional timeout
    std::optional<ResultType> GetResult(TaskStatus& status) {
        std::unique_lock<std::mutex> lock(mutex_);
        if (timeout == std::chrono::milliseconds::max()) {
            cv.wait(lock, [this] { return !is_running; });
        } else if (!cv.wait_for(lock, timeout, [this] { return !is_running; })) {
            status = TaskStatus::TIMEOUT;
            return std::nullopt;
        }

        if (is_cancelled) {
            status = TaskStatus::CANCELLED;
            return std::nullopt;
        }

        status = error_code;
        return result;
    }

    // Cancel the running task
    void Cancel() {
        Stop();
    }

protected:
    // Derived classes must implement this method to define the background task
    virtual ResultType Execute() = 0;

    // Method to execute the task in a thread
    void ExecuteTask() {
        ResultType task_result;

        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (is_cancelled) {
                is_running = false;
                error_code = TaskStatus::CANCELLED;
                cv.notify_all();
                return;
            }
        }

        task_result = Execute();

        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (!is_cancelled) {
                result = task_result;
                error_code = TaskStatus::SUCCESS;
            } else {
                error_code = TaskStatus::CANCELLED;
            }
            is_running = false;
        }
        cv.notify_all();

        // Invoke the callback, if provided
        if (on_complete) {
            on_complete(error_code);
        }
    }

    TaskCallback on_complete;  // Store the callback

protected:
    mutable std::mutex mutex_;
    std::condition_variable cv;
    std::optional<ResultType> result;
    TaskStatus error_code = TaskStatus::SUCCESS;
    std::chrono::milliseconds timeout; // Timeout for the task

    std::atomic<bool> is_running;
    std::atomic<bool> is_cancelled;
    std::thread task_thread;
};


#endif //POTOPOTO_BACKGROUNDTASK_H
