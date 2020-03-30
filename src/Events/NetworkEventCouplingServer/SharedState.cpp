#include "SharedState.h"

#include <K/Core/Log.h>

using std::unique_lock;
using std::mutex;
using std::to_string;
using K::Core::Log;

namespace K {
namespace Events {

NetworkEventCouplingServer::SharedState::SharedState()
        : couplingFinished_(false),
          workerFinished_(false),
          shutDownRequested_(false) {
    // Nop.
}

void NetworkEventCouplingServer::SharedState::RequestShutDown() {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    shutDownRequested_ = true;
    stateChanged_.notify_all();
}    // ......................................................................................... critical section, end.

bool NetworkEventCouplingServer::SharedState::ShutDownRequested() {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    return shutDownRequested_;
}    // ......................................................................................... critical section, end.

void NetworkEventCouplingServer::SharedState::PrepareForCoupling() {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    couplingFinished_ = false;
}    // ......................................................................................... critical section, end.

void NetworkEventCouplingServer::SharedState::WaitForCouplingFinished() {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    while (!couplingFinished_ && !shutDownRequested_) {
        stateChanged_.wait(critical);
    }
}    // ......................................................................................... critical section, end.

void NetworkEventCouplingServer::SharedState::WaitForWorkerFinished() {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    while (!workerFinished_) {
        stateChanged_.wait(critical);
    }
}    // ......................................................................................... critical section, end.

void NetworkEventCouplingServer::SharedState::OnCompletion(int completionId) {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    if (completionId == couplingCompletionId) {
        couplingFinished_ = true;
        Log::Print(Log::Level::Debug, this, [&]{ return "coupling signalled completion"; });
    }
    else if (completionId == workerCompletionId) {
        workerFinished_ = true;
    }
    stateChanged_.notify_all();
}    // ......................................................................................... critical section, end.

}    // Namespace Events.
}    // Namespace K.

