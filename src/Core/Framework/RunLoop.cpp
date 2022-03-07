////    ////
////   ////     K Crossplatform C++ Assets
////  ////      (C) Copyright Kai Hergenröther
//// ////
////////        - Core / Framework -
//// ////
////  ////
////   ////
////    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <K/Core/Framework/RunLoop.h>

#include <K/Core/Log.h>

using std::mutex;
using std::unique_lock;
using K::Core::Log;

namespace K {
namespace Core {
namespace Framework {

RunLoop::RunLoop()
        : terminationRequested_(false) {
    // Nop.
}

void RunLoop::Run() {
    Log::Print(Log::Level::Debug, this, [&]{ return "entering run loop"; });

    bool done = false;
    while (!done) {
        ClientInterface *clientToActivate = nullptr;
        {
            unique_lock<mutex> critical(lock_);    // Critical section..................................................
            while (!clientToActivate && !done) {
                if (terminationRequested_) {
                    done = true;
                    terminationRequested_ = false;
                } else if (!clientsToActivate_.empty()) {
                    int client = clientActivationOrder_.front();
                    clientActivationOrder_.pop_front();
                    clientsToActivate_.erase(client);
                    clientToActivate = clients_[client].client;
                } else {
                    stateChanged_.wait(critical);
                }
            }
        }    // ................................................................................. critical section, end.

        // If we have one, we only have a weak reference to the client to activate, yet client removal exclusively
        // happens on the loop thread. Since we're on the loop thread, we can be sure the client reference is valid for
        // the call...
        if (clientToActivate) {
            clientToActivate->Activate();
        }
    }

    Log::Print(Log::Level::Debug, this, [&]{ return "exiting run loop"; });
}

void RunLoop::RequestTermination() {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    terminationRequested_ = true;
    stateChanged_.notify_all();
}    // ......................................................................................... critical section, end.

int RunLoop::AddClient(ClientInterface *client) {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    int slot;
    if (freeClientSlots_.empty()) {
        slot = static_cast<int>(clients_.size());
        clients_.push_back(ClientInfo());
    } else {
        slot = freeClientSlots_.front();
        freeClientSlots_.pop_front();
    }

    ClientInfo &info = clients_[slot];
    info.client = client;
    return slot;
}    // ......................................................................................... critical section, end.

void RunLoop::RemoveClient(int client) {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    ClearPendingActivation(client, critical);
    clients_[client].Reset();
    freeClientSlots_.push_back(client);
}    // ......................................................................................... critical section, end.

void RunLoop::RequestActivation(int client) {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    if (!clientsToActivate_.contains(client)) {
        clientsToActivate_.insert(client);
        clientActivationOrder_.push_back(client);
        stateChanged_.notify_all();
    }
}    // ......................................................................................... critical section, end.

// Expects lock to be held.
void RunLoop::ClearPendingActivation(int client, unique_lock<mutex> &critical) {
    (void)critical;
    if (clientsToActivate_.contains(client)) {
        clientsToActivate_.erase(client);
        std::deque<int> newOrder;
        for (int aClient : clientActivationOrder_) {
            if (aClient != client) {
                newOrder.push_back(aClient);
            }
        }
        clientActivationOrder_.swap(newOrder);
    }
}

RunLoop::ClientInfo::ClientInfo() {
    Reset();
}

void RunLoop::ClientInfo::Reset() {
    client = nullptr;
}

}    // Namespace Framework.
}    // Namespace Core.
}    // Namespace K.
