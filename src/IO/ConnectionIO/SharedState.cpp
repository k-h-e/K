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

ConnectionIO::SharedState::SharedState(int pipe)
        : pipe_(pipe),
          registrationRunning_(false),
          unregistrationRunning_(false),
          shutDownRequested_(false),
          workerFinished_(false) {
    error_ = (pipe_ == -1);
    Log::Print(Log::Level::Debug, this, [&]{ return "pipe=" + to_string(pipe_); });
}

ConnectionIO::SharedState::~SharedState() {
    if (pipe_ != -1) {
        close(pipe_);
    }
}

bool ConnectionIO::SharedState::Register(ClientInterface *client, int fd) {
    assert(client);
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    if (error_ || (fd < 0)) {
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

    registrationInfo_    = RegistrationInfo(client, fd);
    registrationRunning_ = true;
    NotifyWorker();

    while (!registrationInfo_.finished && !error_) {
        stateChanged_.wait(critical);
    }
    if (error_) {
        return false;
    }

    registrationRunning_ = false;

    return registrationInfo_.success;
}    // ......................................................................................... critical section, end.

void ConnectionIO::SharedState::Unregister(ClientInterface *client, bool *outError)  {
    assert(client);
    *outError = true;

    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
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

    unregistrationInfo_    = UnregistrationInfo(client);
    unregistrationRunning_ = true;
    NotifyWorker();

    while (!unregistrationInfo_.finished && !error_) {
        stateChanged_.wait(critical);
    }
    if (error_) {
        DoShutDown(&critical);
        return;
    }

    *outError              = unregistrationInfo_.finalStreamError;
    unregistrationRunning_ = false;
}    // ......................................................................................... critical section, end.

void ConnectionIO::SharedState::SetClientCanRead(ClientInterface *client) {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    clientsReadyToRead_.push_back(client);
    NotifyWorker();
}    // ......................................................................................... critical section, end.

void ConnectionIO::SharedState::SetClientCanWrite(ClientInterface *client) {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    clientsReadyToWrite_.push_back(client);
    NotifyWorker();
}    // ......................................................................................... critical section, end.

void ConnectionIO::SharedState::ShutDown()  {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    DoShutDown(&critical);
}    // ......................................................................................... critical section, end.

void ConnectionIO::SharedState::GetWork(WorkInfo *outInfo) {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................

    outInfo->Clear();

    outInfo->shutDownRequested = shutDownRequested_;

    if (registrationInfo_.client) {
        outInfo->registrationInfo = registrationInfo_;
        registrationInfo_ = RegistrationInfo();
    }

    if (unregistrationInfo_.client) {
        outInfo->unregistrationInfo = unregistrationInfo_;
        unregistrationInfo_ = UnregistrationInfo();
    }

    for (ClientInterface *client : clientsReadyToRead_) {
        outInfo->clientsReadyToRead.push_back(client);
    }
    clientsReadyToRead_.clear();

    for (ClientInterface *client : clientsReadyToWrite_) {
        outInfo->clientsReadyToWrite.push_back(client);
    }
    clientsReadyToWrite_.clear();

}    // ......................................................................................... critical section, end.

void ConnectionIO::SharedState::SetErrorState() {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    if (!error_) {
        Log::Print(Log::Level::Critical, this, []{ return "entering error state"; });
    }
    error_ = true;
    stateChanged_.notify_all();

}    // ......................................................................................... critical section, end.

void ConnectionIO::SharedState::OnClientRegistered(bool success) {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    registrationInfo_.success  = success;
    registrationInfo_.finished = true;
    stateChanged_.notify_all();
}    // ......................................................................................... critical section, end.

void ConnectionIO::SharedState::OnClientUnregistered(bool finalStreamError) {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    unregistrationInfo_.finalStreamError = finalStreamError;
    unregistrationInfo_.finished         = true;
    stateChanged_.notify_all();
}    // ......................................................................................... critical section, end.

void ConnectionIO::SharedState::OnCompletion(int completionId) {
    (void)completionId;

    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    workerFinished_ = true;
    stateChanged_.notify_all();
}    // ......................................................................................... critical section, end.

// Lock assumed to be held.
void ConnectionIO::SharedState::NotifyWorker() {
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
void ConnectionIO::SharedState::DoShutDown(unique_lock<mutex> *critical) {
    shutDownRequested_ = true;
    NotifyWorker();
    while (!workerFinished_) {
        stateChanged_.wait(*critical);
    }
}

}    // Namespace IO.
}    // Namespace K.

