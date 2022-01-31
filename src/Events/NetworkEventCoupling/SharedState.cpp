#include "SharedState.h"

#include <K/Core/CompletionHandlerInterface.h>
#include <K/Core/Log.h>
#include <K/IO/TcpConnection.h>
#include <K/Events/EventLoopHub.h>

using std::shared_ptr;
using std::unique_lock;
using std::mutex;
using K::Core::CompletionHandlerInterface;
using K::Core::Log;
using K::IO::TcpConnection;

namespace K {
namespace Events {

NetworkEventCoupling::SharedState::SharedState(
    const shared_ptr<CompletionHandlerInterface> &completionHandler, int completionId,
    const shared_ptr<EventLoopHub> &hub, int hubClientId)
        : completionHandler_(completionHandler),
          completionId_(completionId),
          keepAlive_(false),
          writerFinished_(false),
          hub_(hub),
          hubClientId_(hubClientId) {
    // Nop.
}

void NetworkEventCoupling::SharedState::RegisterTcpConnection(TcpConnection *connection) {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    tcpConnection_ = connection;
}    // ......................................................................................... critical section, end.


void NetworkEventCoupling::SharedState::ShutDown() {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    hub_->RequestShutDown(hubClientId_);
    while (!writerFinished_) {
        stateChanged_.wait(critical);
    }
}    // ......................................................................................... critical section, end.

void NetworkEventCoupling::SharedState::OnKeepAlive() {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................

    // TESTING...
    static int counter = 0;
    ++counter;
    if (counter < 10) {
        keepAlive_ = true;
        Log::Print(Log::Level::Debug, this, [&]{ return "keep-alive"; });
    }
}    // ......................................................................................... critical section, end.

void NetworkEventCoupling::SharedState::OnCompletion(int completionId) {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................

    if (completionId == writerCompletionId) {
        writerFinished_ = true;
    }

    if (writerFinished_) {
        if (completionHandler_) {
            completionHandler_->OnCompletion(completionId_);
            completionHandler_.reset();
        }
    }

    stateChanged_.notify_all();
}    // ......................................................................................... critical section, end.

void NetworkEventCoupling::SharedState::OnTimer(int timer) {
    (void)timer;
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    if (keepAlive_) {
        Log::Print(Log::Level::Debug, this, [&]{ return "keep-alive OK"; });
    } else {
        Log::Print(Log::Level::Error, this, [&]{ return "no keep-alive received for too long, disconnecting..."; });
        hub_->RequestShutDown(hubClientId_);
        if (tcpConnection_) {
            tcpConnection_->TriggerShutDown();
        }
    }
    keepAlive_ = false;
}    // ......................................................................................... critical section, end.

}    // Namespace Events.
}    // Namespace K.

