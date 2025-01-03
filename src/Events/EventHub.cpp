/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#include <K/Events/EventHub.h>

#include <cassert>
#include <cstdio>
#include <K/Core/Buffer.h>
#include <K/Core/Log.h>
#include <K/Events/Event.h>

using std::make_shared;
using std::mutex;
using std::optional;
using std::to_string;
using std::unique_lock;
using std::unique_ptr;
using std::chrono::milliseconds;
using K::Core::Buffer;
using K::Core::Log;

namespace K {
namespace Events {

EventHub::EventHub()
        : shutDownRequested_{false} {
    // Nop.
}

int EventHub::RegisterEventLoop() {
    unique_lock<mutex> critical{lock_};    // Critical section..........................................................
    int id;
    if (unusedLoopSlots_.empty()) {
        id = static_cast<int>(loops_.size());
        loops_.push_back(LoopInfo());
    } else {
        id = unusedLoopSlots_.top();
        unusedLoopSlots_.pop();
    }

    LoopInfo &loopInfo { loops_[id] };
    loopInfo.inUse = true;
    Log::Print(Log::Level::Debug, this, [&]{ return "registered event loop, id=" + to_string(id); });

    return id;
}    // ......................................................................................... critical section, end.

void EventHub::UnregisterEventLoop(int clientLoopId) {
    unique_lock<mutex> critical{lock_};    // Critical section..........................................................
    LoopInfo *info { GetLoopInfo(critical, clientLoopId) };
    if (info) {
        info->Reset();
        unusedLoopSlots_.push(clientLoopId);
        Log::Print(Log::Level::Debug, this, [&]{ return "deregistered event loop " + to_string(clientLoopId); });
    }
}    // ......................................................................................... critical section, end.

void EventHub::RegisterHandler(int clientLoopId, HandlerInterface *handler) {
    unique_lock<mutex> critical{lock_};    // Critical section..........................................................
    LoopInfo *info { GetLoopInfo(critical, clientLoopId) };
    if (info) {
        info->handler = handler;
    }
}    // ......................................................................................... critical section, end.

bool EventHub::RegisterEventIdToSlotMapping(size_t id, int slot) {
    unique_lock<mutex> critical{lock_};    // Critical section..........................................................
    auto iter { eventIdToSlotMap_.find(id) };
    if (iter != eventIdToSlotMap_.end()) {
        if (iter->second == slot) {
            return true;
        } else {
            return false;
        }
    }

    eventIdToSlotMap_[id] = slot;
    return true;
}    // ......................................................................................... critical section, end.

bool EventHub::Sync(int clientLoopId, unique_ptr<Buffer> *buffer) {
    unique_lock<mutex> critical{lock_};    // Critical section..........................................................
    if ((*buffer)->DataSize()) {
        DoSubmit(critical, 0, (*buffer)->Data(), (*buffer)->DataSize(), false);
        (*buffer)->Clear();
    }

    LoopInfo *info { GetLoopInfo(critical, clientLoopId) };
    if (!info || info->shutDownRequested || shutDownRequested_) {
        return false;
    } else {
        if (info->buffer->DataSize()) {
            info->buffer.swap(*buffer);
        }
        return true;
    }
}    // ......................................................................................... critical section, end.

bool EventHub::Sync(int clientLoopId, unique_ptr<Buffer> *buffer, optional<milliseconds> timeout) {
    unique_lock<mutex> critical{lock_};    // Critical section..........................................................
    if ((*buffer)->DataSize()) {
        DoSubmit(critical, 0, (*buffer)->Data(), (*buffer)->DataSize(), false);
        (*buffer)->Clear();
    }

    bool didWait { false };
    while (true) {
        LoopInfo *info { GetLoopInfo(critical, clientLoopId) };
        if (!info || info->shutDownRequested || shutDownRequested_) {
            return false;
        } else if (info->buffer->DataSize()) {
            info->buffer.swap(*buffer);
            return true;
        } else {
            if (timeout) {
                if (didWait) {
                    return true;
                } else {
                    info->waiting = true;
                    info->stateChanged->wait_for(critical, *timeout);
                    info->waiting = false;
                    didWait = true;
                }
            } else {
                info->waiting = true;
                info->stateChanged->wait(critical);
                info->waiting = false;
            }
        }
    }
}    // ......................................................................................... critical section, end.

void EventHub::Submit(int clientLoopId, const void *data, int dataSize, bool onlyDeliverToOthers) {
    unique_lock<mutex> critical{lock_};    // Critical section..........................................................
    DoSubmit(critical, clientLoopId, data, dataSize, onlyDeliverToOthers);
}    // ......................................................................................... critical section, end.

void EventHub::Post(int clientLoopId, const Event &event, bool onlyDeliverToOthers) {
    unique_lock<mutex> critical{lock_};    // Critical section..........................................................
    auto iter = eventIdToSlotMap_.find(event.Type().id);
    assert(iter != eventIdToSlotMap_.end());
    int slot {iter->second };
    eventsToSchedule_ << slot;
    event.Serialize(&eventsToSchedule_);
    DoSubmit(critical, clientLoopId, eventsToSchedule_.Data(), eventsToSchedule_.DataSize(), onlyDeliverToOthers);
    eventsToSchedule_.Clear();
}    // ......................................................................................... critical section, end.

void EventHub::RequestShutDown() {
    unique_lock<mutex> critical{lock_};    // Critical section..........................................................
    shutDownRequested_ = true;
    for (auto &loopInfo : loops_) {
        if (loopInfo.inUse) {
            loopInfo.stateChanged->notify_all();
            if (loopInfo.handler) {
                loopInfo.handler->OnEventsAvailable();
            }
        }
    }
}    // ......................................................................................... critical section, end.

void EventHub::RequestShutDown(int clientLoopId) {
    unique_lock<mutex> critical{lock_};    // Critical section..........................................................
    LoopInfo *loopInfo { GetLoopInfo(critical, clientLoopId) };
    if (loopInfo) {
        loopInfo->shutDownRequested = true;
        loopInfo->stateChanged->notify_all();
        if (loopInfo->handler) {
            loopInfo->handler->OnEventsAvailable();
        }
        Log::Print(Log::Level::Debug, this, [&]{
            return "shutdown requested for event loop " + to_string(clientLoopId);
        });
    }
}    // ......................................................................................... critical section, end.

void EventHub::Post(const Event &event) {
    Post(0, event, false);
}

// ---

// Expects lock to be held.
void EventHub::DoSubmit(unique_lock<mutex> &critical, int clientLoopId, const void *data, int dataSize,
                        bool onlyDeliverToOthers) {
    (void) critical;
    int num { static_cast<int>(loops_.size()) };
    for (int i = 0; i < num; ++i) {
        LoopInfo &loopInfo { loops_[i] };
        if (loopInfo.inUse) {
            bool wasEmpty { (loopInfo.buffer->DataSize() == 0) };
            if (!(onlyDeliverToOthers && (i == clientLoopId))) {
                loopInfo.buffer->Append(data, dataSize);
                if (loopInfo.waiting) {
                    loopInfo.stateChanged->notify_all();
                }
                if (loopInfo.handler && wasEmpty) {
                    loopInfo.handler->OnEventsAvailable();
                }
            }
        }
    }
}

// Expects lock to be held.
EventHub::LoopInfo *EventHub::GetLoopInfo(unique_lock<mutex> &critical, int clientLoopId) {
    (void)critical;
    if ((clientLoopId >= 0) && (clientLoopId < static_cast<int>(loops_.size()))) {
        LoopInfo &info { loops_[clientLoopId] };
        if (info.inUse) {
            return &info;
        }
    }

    return nullptr;
}

}    // Namespace Events.
}    // Namespace K.
