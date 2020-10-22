#include "SharedState.h"

#include <unistd.h>
#include <cstring>
#include <K/Core/Log.h>

using std::shared_ptr;
using std::unique_lock;
using std::mutex;
using std::string;
using std::to_string;
using K::Core::Log;

namespace K {
namespace IO {

IO::SharedState::SharedState(int pipe)
        : shutDownRequested_(false),
          workerFinished_(false),
          pipe_(pipe) {
    error_ = (pipe_ == -1);
    Log::Print(Log::Level::Debug, this, [&]{ return "pipe=" + to_string(pipe_); });
}

IO::SharedState::~SharedState() {
    if (pipe_ != -1) {
        close(pipe_);
    }
}

bool IO::SharedState::Register(int fd, ReadHandlerInterface *reader, WriteHandlerInterface *writer) {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    if (!error_) {
        // TODO...
        Log::Print(Log::Level::Debug, this, [&]{ return "registered fd=" + to_string(fd); });
        return true;
    }

    return false;
}    // ......................................................................................... critical section, end.

void IO::SharedState::Unregister(int fd)  {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    if (!error_) {
        Log::Print(Log::Level::Debug, this, [&]{ return "unregistered fd=" + to_string(fd); });
    }
}    // ......................................................................................... critical section, end.

void IO::SharedState::ShutDown()  {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    shutDownRequested_ = true;
    WakeWorker();
    while (!workerFinished_) {
        stateChanged_.wait(critical);
    }
}    // ......................................................................................... critical section, end.

void IO::SharedState::SetErrorState() {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................

    // TODO...

}    // ......................................................................................... critical section, end.

void IO::SharedState::OnCompletion(int completionId) {
    (void)completionId;

    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    workerFinished_ = true;
    stateChanged_.notify_all();
}    // ......................................................................................... critical section, end.

// Lock assumed to be held.
void IO::SharedState::WakeWorker() {
    if (pipe_ != -1) {
        Log::Print(Log::Level::Debug, this, [&]{ return "waking worker, pipe=" + to_string(pipe_); });
        uint8_t dummyByte = 0u;
        int numWritten = write(pipe_, &dummyByte, 1);
        if (numWritten == 1) {
            Log::Print(Log::Level::Debug, this, []{ return "    wrote byte to pipe"; });
        }
        else {
            Log::Print(Log::Level::Debug, this, [&]{ return "    write() returned " + to_string(numWritten); });
            Log::Print(Log::Level::Debug, this, [&]{ return string("    errno: ") + std::strerror(errno); });
        }
    }
}

}    // Namespace IO.
}    // Namespace K.

