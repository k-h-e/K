#include "SharedState.h"

#include <K/Core/Log.h>
#include <K/Core/Timers.h>
#include <K/Events/NetworkEventCoupling.h>

using std::make_shared;
using std::mutex;
using std::shared_ptr;
using std::string;
using std::to_string;
using std::unique_lock;
using K::Core::Log;
using K::Core::ThreadPool;
using K::Core::Timers;
using K::IO::ConnectionIO;
using K::IO::ListenSocket;
using K::IO::TcpConnection;

namespace K {
namespace Events {

NetworkEventCouplingServer::SharedState::SharedState(
    const string &protocolVersion, const shared_ptr<EventLoopHub> &hub,
    const shared_ptr<ConnectionIO> &connectionIO, const shared_ptr<ThreadPool> &threadPool,
    const shared_ptr<Timers> &timers)
        : hub_(hub),
          connectionIO_(connectionIO),
          threadPool_(threadPool),
          timers_(timers),
          protocolVersion_(protocolVersion),
          unpauseTimer_(false),
          shutDown_(false) {
    // Nop.
}

void NetworkEventCouplingServer::SharedState::SetTimer(int timer) {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    timer_ = timer;
    if (unpauseTimer_) {
        timers_->PauseTimer(*timer_, false);
        unpauseTimer_ = false;
    }
}    // ......................................................................................... critical section, end.

void NetworkEventCouplingServer::SharedState::ShutDown() {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    shutDown_ = true;
}    // ......................................................................................... critical section, end.

void NetworkEventCouplingServer::SharedState::OnListenSocketAcceptedConnection(
        const shared_ptr<TcpConnection> &connection) {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    if (!shutDown_) {
        coupling_ = make_shared<NetworkEventCoupling>(connection, protocolVersion_, hub_, nullptr, 0, threadPool_,
                                                      timers_);
    }
}    // ......................................................................................... critical section, end.

void NetworkEventCouplingServer::SharedState::OnListenSocketErrorState() {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    if (!shutDown_) {
        Log::Print(Log::Level::Warning, this, [&]{ return "listen socket entered error state!"; });
        if (timer_) {
            timers_->PauseTimer(*timer_, false);
        } else {
            unpauseTimer_ = true;
        }
    }
}    // ......................................................................................... critical section, end.

}    // Namespace Events.
}    // Namespace K.

