#include "SharedState.h"

#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <cassert>
#include <K/Core/Log.h>
#include "WorkInfo.h"

using std::shared_ptr;
using std::unique_lock;
using std::mutex;
using std::string;
using std::to_string;
using K::Core::Log;

namespace K {
namespace IO {

IO::SharedState::SharedState(int pipe)
        : pipe_(pipe),
          registrationRunning_(false),
          registrationSucceeded_(false),
          registrationFailed_(false),
          unregistrationRunning_(false),
          fileDescriptorToUnregister_(-1),
          unregistrationFinished_(false),
          shutDownRequested_(false),
          workerFinished_(false) {
    error_ = (pipe_ == -1);
    Log::Print(Log::Level::Debug, this, [&]{ return "pipe=" + to_string(pipe_); });
}

IO::SharedState::~SharedState() {
    if (pipe_ != -1) {
        close(pipe_);
    }
}

bool IO::SharedState::Register(int fd, ClientInterface *client) {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    if (error_ || (fd < 0) || !client) {
        return false;
    }

    bool switchedToNonBlocking = false;
    int flags = fcntl(fd, F_GETFL);
    if (flags != -1) {
        if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) != -1) {
            switchedToNonBlocking = true;
        }
    }
    if (!switchedToNonBlocking) {
        return false;
    }

    while (registrationRunning_ && !error_) {
        stateChanged_.wait(critical);
    }
    if (error_) {
        return false;
    }

    registrationRunning_ = true;

    registrationInfo_      = RegistrationInfo(fd, client);
    registrationSucceeded_ = false;
    registrationFailed_    = false;
    NotifyWorker();

    while (!registrationSucceeded_ && !registrationFailed_ && !error_) {
        stateChanged_.wait(critical);
    }
    if (error_) {
        return false;
    }

    registrationRunning_ = false;

    return registrationSucceeded_;
}    // ......................................................................................... critical section, end.

void IO::SharedState::Unregister(int fd)  {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    if (fd < 0) {
        return;
    }
    if (error_) {
        DoShutDown(&critical);
        return;
    }

    while (unregistrationRunning_ && !error_) {
        stateChanged_.wait(critical);
    }
    if (error_) {
        DoShutDown(&critical);
        return;
    }

    unregistrationRunning_ = true;

    fileDescriptorToUnregister_ = fd;
    unregistrationFinished_     = false;
    NotifyWorker();

    while (!unregistrationFinished_ && !error_) {
        stateChanged_.wait(critical);
    }
    if (error_) {
        DoShutDown(&critical);
        return;
    }

    unregistrationRunning_ = false;
}    // ......................................................................................... critical section, end.

void IO::SharedState::SetClientCanRead(int fd) {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    clientsReadyToRead_.push_back(fd);
    NotifyWorker();
}    // ......................................................................................... critical section, end.

void IO::SharedState::SetClientCanWrite(int fd) {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    clientsReadyToWrite_.push_back(fd);
    NotifyWorker();
}    // ......................................................................................... critical section, end.

void IO::SharedState::ShutDown()  {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    DoShutDown(&critical);
}    // ......................................................................................... critical section, end.

void IO::SharedState::GetWork(WorkInfo *outInfo) {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................

    outInfo->Clear();

    outInfo->shutDownRequested = shutDownRequested_;

    if (registrationInfo_.fileDescriptor >= 0) {
        outInfo->registrationInfo = registrationInfo_;
        registrationInfo_ = RegistrationInfo();
    }

    if (fileDescriptorToUnregister_ >= 0) {
        outInfo->fileDescriptorToUnregister = fileDescriptorToUnregister_;
        fileDescriptorToUnregister_ = -1;
    }

    for (int fd : clientsReadyToRead_) {
        outInfo->clientsReadyToRead.push_back(fd);
    }
    clientsReadyToRead_.clear();

    for (int fd : clientsReadyToWrite_) {
        outInfo->clientsReadyToWrite.push_back(fd);
    }
    clientsReadyToWrite_.clear();

}    // ......................................................................................... critical section, end.

void IO::SharedState::SetErrorState() {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    if (!error_) {
        Log::Print(Log::Level::Critical, this, []{ return "entering error state"; });
    }
    error_ = true;
    stateChanged_.notify_all();

}    // ......................................................................................... critical section, end.

void IO::SharedState::OnClientRegistered(bool success) {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    if (success) {
        registrationSucceeded_ = true;
    }
    else {
        registrationFailed_ = true;
    }
    stateChanged_.notify_all();
}    // ......................................................................................... critical section, end.

void IO::SharedState::OnClientUnregistered() {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    unregistrationFinished_ = true;
    stateChanged_.notify_all();
}    // ......................................................................................... critical section, end.

void IO::SharedState::OnCompletion(int completionId) {
    (void)completionId;

    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    workerFinished_ = true;
    stateChanged_.notify_all();
}    // ......................................................................................... critical section, end.

// Lock assumed to be held.
void IO::SharedState::NotifyWorker() {
    if (pipe_ != -1) {
        uint8_t dummyByte = 0u;
        while (true) {
            int numWritten = write(pipe_, &dummyByte, 1);
            if (numWritten == 1) {
                return;
            }
            else if (numWritten == -1) {
                if (errno == EINTR) {
                    // Nop.
                }
                else {
                    assert(false);
                }
            }
        }
    }
}

// Lock assumed to be held.
void IO::SharedState::DoShutDown(unique_lock<mutex> *critical) {
    shutDownRequested_ = true;
    NotifyWorker();
    while (!workerFinished_) {
        stateChanged_.wait(*critical);
    }
}

}    // Namespace IO.
}    // Namespace K.

