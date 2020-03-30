#include "SharedState.h"

using std::shared_ptr;
using std::mutex;
using std::unique_lock;

namespace K {
namespace Events {

NetworkEventCouplingClient::SharedState::SharedState()
        : connecting_(false) {
    // Nop.
}

void NetworkEventCouplingClient::SharedState::PrepareToConnect() {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    coupling_.reset();
    connecting_ = true;
}    // ......................................................................................... critical section, end.

void NetworkEventCouplingClient::SharedState::WaitForWorker() {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    while (connecting_) {
        stateChanged_.wait(critical);
    }
}    // ......................................................................................... critical section, end.

void NetworkEventCouplingClient::SharedState::Disconnect() {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    coupling_.reset();
}    // ......................................................................................... critical section, end.

void NetworkEventCouplingClient::SharedState::OnCouplingCreated(const shared_ptr<NetworkEventCoupling> &coupling) {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    coupling_ = coupling;
}    // ......................................................................................... critical section, end.

void NetworkEventCouplingClient::SharedState::OnCompletion(int completionId) {
    (void)completionId;

    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    connecting_ = false;
    stateChanged_.notify_all();
}    // ......................................................................................... critical section, end.

}    // Namespace Events.
}    // Namespace K.

