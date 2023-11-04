/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's C++ Crossplatform Assets                                                                        ///
//    (C) Copyright Kai Hergenröther. All rights reserved.                                                 //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#include "SharedState.h"

#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <cassert>
#include <K/Core/Log.h>
#include <K/IO/IOTools.h>
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
          registrationOperationRunning_(false),
          error_(false),
          shutDownRequested_(false),
          workerFinished_(false) {
    // If the pipe is bad, error state will be regularly raised by the worker.
}

ConnectionIO::SharedState::~SharedState() {
    if (pipe_ != -1) {
        (void)IOTools::CloseFileDescriptor(pipe_, this);
    }
}

bool ConnectionIO::SharedState::Register(const shared_ptr<ClientInterface> &client, int fd) {
    assert(client);
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    if (error_ || (fd < 0)) {
        Log::Print(Log::Level::Warning, this, [&]{ return "failed to register client for fd " + to_string(fd)
            + ", initial checks failed, error_state=" + to_string(error_); });
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
        Log::Print(Log::Level::Warning, this, [&]{ return "failed to register client for fd " + to_string(fd)
            + ", could not switch fd to non-blocking"; });
        return false;
    }

    while (registrationOperationRunning_) {
        if (error_) {
            Log::Print(Log::Level::Warning, this, [&]{ return "failed to register client for fd " + to_string(fd)
                + ", entered error state while waiting for other operation to finish"; });
            return false;
        }
        stateChanged_.wait(critical);
    }

    registrationInfo_             = RegistrationInfo(client, fd);
    registrationOperationRunning_ = true;
    NotifyWorker();
    while (!registrationInfo_.finished) {
        if (error_) {
            Log::Print(Log::Level::Warning, this, [&]{ return "failed to register client for fd " + to_string(fd)
                + ", entered error state while waiting for registration to finish"; });
            registrationOperationRunning_ = false;
            registrationInfo_             = RegistrationInfo();
            return false;
        }
        stateChanged_.wait(critical);
    }

    bool success = registrationInfo_.success;

    registrationOperationRunning_ = false;
    registrationInfo_             = RegistrationInfo();

    if (!success) {
        Log::Print(Log::Level::Warning, this, [&]{ return "failed to register client for fd " + to_string(fd)
            + ", worker reported failure"; });
    }
    return success;
}    // ......................................................................................... critical section, end.

void ConnectionIO::SharedState::Unregister(const shared_ptr<ClientInterface> &client, bool *outFinalStreamError)  {
    assert(client);
    *outFinalStreamError = true;

    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    while (registrationOperationRunning_) {
        if (error_) {
            Log::Print(Log::Level::Warning, this, []{
                return "unregistered client, but entered error state while waiting for other operation to finish";
            });
            DoShutDown(&critical);
            return;
        }
        stateChanged_.wait(critical);
    }

    unregistrationInfo_           = UnregistrationInfo(client);
    registrationOperationRunning_ = true;
    NotifyWorker();
    while (!unregistrationInfo_.finished) {
        if (error_) {
            Log::Print(Log::Level::Warning, this, []{
                return "unregistered client, but entered error state while waiting for unregistration to finish";
            });
            DoShutDown(&critical);
            unregistrationInfo_           = UnregistrationInfo();
            registrationOperationRunning_ = true;
            return;
        }
        stateChanged_.wait(critical);
    }

    Log::Print(Log::Level::Debug, this, [&]{
        return "unregistered client, final_stream_error=" + to_string(unregistrationInfo_.finalStreamError);
    });

    *outFinalStreamError          = unregistrationInfo_.finalStreamError;

    unregistrationInfo_           = UnregistrationInfo();
    registrationOperationRunning_ = false;
}    // ......................................................................................... critical section, end.

void ConnectionIO::SharedState::SetClientCanRead(ClientInterface *client) {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    if (!error_) {
        clientsReadyToRead_.push_back(client);
        NotifyWorker();
    }
}    // ......................................................................................... critical section, end.

void ConnectionIO::SharedState::SetClientCanWrite(ClientInterface *client) {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    if (!error_) {
        clientsReadyToWrite_.push_back(client);
        NotifyWorker();
    }
}    // ......................................................................................... critical section, end.

void ConnectionIO::SharedState::RequestCustomCall(ClientInterface *client) {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    if (!error_) {
        clientsWithCustomCallRequested_.push_back(client);
        NotifyWorker();
    }
}    // ......................................................................................... critical section, end.

void ConnectionIO::SharedState::RequestErrorState(ClientInterface *client) {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    if (!error_) {
        clientsWithErrorStateRequested_.push_back(client);
        NotifyWorker();
    }
}    // ......................................................................................... critical section, end.

void ConnectionIO::SharedState::ShutDown()  {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    DoShutDown(&critical);
}    // ......................................................................................... critical section, end.

void ConnectionIO::SharedState::GetWork(WorkInfo *outInfo) {
    outInfo->Clear();
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    outInfo->shutDownRequested = shutDownRequested_;

    if (!error_) {
        if (registrationInfo_.client && !registrationInfo_.acceptedByWorker) {
            registrationInfo_.acceptedByWorker = true;
            outInfo->registrationInfo = registrationInfo_;
        }

        if (unregistrationInfo_.client && !unregistrationInfo_.acceptedByWorker) {
            unregistrationInfo_.acceptedByWorker = true;
            outInfo->unregistrationInfo = unregistrationInfo_;
        }

        for (ClientInterface *client : clientsReadyToRead_) {
            outInfo->clientsReadyToRead.push_back(client);
        }
        clientsReadyToRead_.clear();

        for (ClientInterface *client : clientsReadyToWrite_) {
            outInfo->clientsReadyToWrite.push_back(client);
        }
        clientsReadyToWrite_.clear();

        for (ClientInterface *client : clientsWithCustomCallRequested_) {
            outInfo->clientsWithCustomCallRequested.push_back(client);
        }
        clientsWithCustomCallRequested_.clear();

        for (ClientInterface *client : clientsWithErrorStateRequested_) {
            outInfo->clientsWithErrorStateRequested.push_back(client);
        }
        clientsWithErrorStateRequested_.clear();
    }
}    // ......................................................................................... critical section, end.

void ConnectionIO::SharedState::OnErrorState() {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    if (!error_) {
        error_ = true;
        stateChanged_.notify_all();
        Log::Print(Log::Level::Warning, this, []{ return "entered error state"; });
    }
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
    Log::Print(Log::Level::Debug, this, []{ return "waiting for worker to finish..."; });

    shutDownRequested_ = true;
    NotifyWorker();
    while (!workerFinished_) {
        stateChanged_.wait(*critical);
    }

    Log::Print(Log::Level::Debug, this, []{ return "worker has finished"; });
}

}    // Namespace IO.
}    // Namespace K.

