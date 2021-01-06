#include "SharedState.h"

#include <K/Core/Log.h>
#include "Runner.h"
#include "SharedRunnerState.h"

using std::shared_ptr;
using std::make_shared;
using std::mutex;
using std::thread;
using std::unique_lock;
using std::to_string;
using K::Core::ActionInterface;
using K::Core::CompletionHandlerInterface;
using K::Core::Log;

namespace K {
namespace Core {

void ThreadPool::SharedState::Run(const shared_ptr<ActionInterface> &action,
                                  const shared_ptr<CompletionHandlerInterface> &completionHandler, int completionId) {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    int slot;
    if (idleThreads_.size()) {
        slot = *idleThreads_.begin();
        idleThreads_.erase(slot);
    }
    else {
        slot = threads_.size();
        threads_.push_back(ThreadInfo());
        ThreadInfo &info = threads_[slot];
        info.sharedRunnerState = make_shared<SharedRunnerState>();
        info.runner            = make_shared<Runner>(slot, *this, info.sharedRunnerState);
        auto aRunner = info.runner;
        info.thread.reset(new thread([=]{ aRunner->Run(); }));
    }

    Log::Print(Log::Level::Debug, this, [=]{ return "executing action on thread " + to_string(slot); });
    ThreadInfo &info = threads_[slot];
    info.sharedRunnerState->Execute(action, completionHandler, completionId);
    LogStats();
}    // ......................................................................................... critical section, end.

void ThreadPool::SharedState::OnCompletion(int completionId) {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    idleThreads_.insert(completionId);
    stateChanged_.notify_all();
    Log::Print(Log::Level::Debug, this, [=]{ return "thread " + to_string(completionId) + " idle"; });
    LogStats();
}    // ......................................................................................... critical section, end.

void ThreadPool::SharedState::ShutDown() {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    while (idleThreads_.size() < threads_.size()) {
        stateChanged_.wait(critical);
    }
    for (ThreadInfo &info : threads_) {
        info.sharedRunnerState->RequestShutDown();
    }
    for (ThreadInfo &info : threads_) {
        info.thread->join();
    }
    Log::Print(Log::Level::Debug, this, [=]{ return "all pool threads terminated"; });
}    // ......................................................................................... critical section, end.

// Expects lock to be held.
void ThreadPool::SharedState::LogStats() {
    Log::Print(Log::Level::Debug, this, [=]{ return to_string(threads_.size()) + " threads, "
        + to_string(idleThreads_.size()) + " idle"; });
}

}    // Namespace Core.
}    // Namespace K.
