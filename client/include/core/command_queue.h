#ifndef GHOSTCLIENT_COMMAND_QUEUE_H
#define GHOSTCLIENT_COMMAND_QUEUE_H

#include <functional>
#include <queue>
#include <mutex>

namespace core {

// A command is simply a function that takes no arguments and returns nothing.
using Command = std::function<void()>;

/// @brief A thread-safe queue for passing commands between threads.
///
/// This is used to send commands from the GUI thread (e.g., "toggle module")
/// to the main game thread for execution.
class CommandQueue {
public:
    /// @brief Gets the singleton instance of the CommandQueue.
    static CommandQueue& get();

    // Delete copy/move constructors to enforce singleton pattern.
    CommandQueue(const CommandQueue&) = delete;
    CommandQueue& operator=(const CommandQueue&) = delete;

    /// @brief Pushes a new command onto the queue. Thread-safe.
    void push(Command command);

    /// @brief Pops a command from the queue if one is available. Thread-safe.
    /// @param command_out The command that was popped.
    /// @return True if a command was popped, false if the queue was empty.
    bool try_pop(Command& command_out);

    /// @brief Processes all pending commands in the queue.
    /// This should be called once per tick from the main game thread.
    void process_commands();

private:
    CommandQueue() = default;
    ~CommandQueue() = default;

    std::queue<Command> queue_;
    std::mutex mutex_;
};

inline CommandQueue& CommandQueue::get() {
    static CommandQueue instance;
    return instance;
}

inline void CommandQueue::push(Command command) {
    std::lock_guard<std::mutex> lock(mutex_);
    queue_.push(std::move(command));
}

inline bool CommandQueue::try_pop(Command& command_out) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (queue_.empty()) {
        return false;
    }
    command_out = std::move(queue_.front());
    queue_.pop();
    return true;
}

inline void CommandQueue::process_commands() {
    std::lock_guard<std::mutex> lock(mutex_);
    while (!queue_.empty()) {
        Command cmd = std::move(queue_.front());
        queue_.pop();
        cmd(); // Execute the command
    }
}

} // namespace core

#endif // GHOSTCLIENT_COMMAND_QUEUE_H
