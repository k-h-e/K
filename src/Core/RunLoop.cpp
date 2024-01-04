/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther.                                                                      //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#include <K/Core/RunLoop.h>

#include <K/Core/Log.h>

using std::mutex;
using std::shared_ptr;
using std::to_string;
using std::unique_lock;
using std::vector;
using K::Core::Log;
using K::Core::Interface;

namespace K {
namespace Core {

RunLoop::RunLoop()
        : terminationRequested_(false) {
    // Nop.
}

void RunLoop::Run() {
    Log::Print(Log::Level::Debug, this, [&]{ return "entering run loop"; });

    vector<shared_ptr<Interface>> deletionDeferredObjects;
    bool done = false;
    while (!done) {
        ClientInterface *clientToActivate  = nullptr;
        int             clientToActivateId = 0;
        bool            deepActivation     = false;
        {
            unique_lock<mutex> critical(lock_);    // Critical section..................................................
            
            if (!deletionDeferredObjects_.empty()) {
                deletionDeferredObjects.swap(deletionDeferredObjects_);
                deletionDeferredObjects_.clear();
                critical.unlock();    // ....................
                Log::Print(Log::Level::Debug, this, [&]{
                    return "releasing " + to_string(deletionDeferredObjects.size()) + " deletion-deferred objects";
                });
                deletionDeferredObjects.clear();
                critical.lock();      // ....................
            }
            
            while (!clientToActivate && !done) {
                if (terminationRequested_) {
                    done = true;
                    terminationRequested_ = false;
                } else if (!clientsToActivate_.empty()) {
                    int client = clientActivationOrder_.front();
                    clientActivationOrder_.pop_front();
                    clientsToActivate_.erase(client);
                    auto &info = clients_[client];
                    clientToActivate   = info.client;
                    clientToActivateId = client;
                    deepActivation     = info.deepActivationRequested;
                    info.deepActivationRequested = false;
                } else {
                    stateChanged_.wait(critical);
                }
            }
        }    // ................................................................................. critical section, end.

        // If we have one, we only have a weak reference to the client to activate, yet client removal exclusively
        // happens on the loop thread. Since we're on the loop thread, we can be sure the client reference is valid for
        // the call...
        if (clientToActivate) {
            Log::Print(Log::Level::DebugDebug, this, [&]{
                return "activating client " + to_string(clientToActivateId) + ", deep=" + to_string(deepActivation);
            });
            clientToActivate->Activate(deepActivation);
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
    Log::Print(Log::Level::Debug, this, [&]{
        return "client added, id=" + to_string(slot) + ", num_clients=" + to_string(NumClients(critical));
    });
    return slot;
}    // ......................................................................................... critical section, end.

void RunLoop::RemoveClient(int client) {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    ClearPendingActivation(client, critical);
    clients_[client].Reset();
    freeClientSlots_.push_back(client);
    Log::Print(Log::Level::Debug, this, [&]{
        return "removed client " + to_string(client) + ", num_clients=" + to_string(NumClients(critical));
    });
}    // ......................................................................................... critical section, end.

void RunLoop::DeferDeletion(const shared_ptr<Interface> &object) {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    deletionDeferredObjects_.push_back(object);
}    // ......................................................................................... critical section, end.


void RunLoop::RequestActivation(int client, bool deepActivation) {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................
    Log::Print(Log::Level::DebugDebug, this, [&]{
        return "RequestActivation(), client=" + to_string(client) + ", deep=" + to_string(deepActivation);
    });
    auto &info = clients_[client];
    if (!clientsToActivate_.contains(client)) {
        clientsToActivate_.insert(client);
        clientActivationOrder_.push_back(client);
        info.deepActivationRequested = deepActivation;
        stateChanged_.notify_all();
    } else {
        if (deepActivation) {
            info.deepActivationRequested = true;
        }
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

// Expects lock to be held.
int RunLoop::NumClients(unique_lock<mutex> &critical) {
    (void)critical;
    return static_cast<int>(clients_.size()) - static_cast<int>(freeClientSlots_.size());
}

RunLoop::ClientInfo::ClientInfo() {
    Reset();
}

void RunLoop::ClientInfo::Reset() {
    client                  = nullptr;
    deepActivationRequested = false;
}

}    // Namespace Core.
}    // Namespace K.
