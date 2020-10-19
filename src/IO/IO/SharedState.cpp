#include "SharedState.h"

#include <K/Core/Log.h>

using std::shared_ptr;
using std::unique_lock;
using std::mutex;
using K::Core::Log;

namespace K {
namespace IO {

IO::SharedState::SharedState() {
    // Nop.
}

bool IO::SharedState::Register(int fd, ReadHandler *reader, WriteHandler *writer) {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................

    // ...

    return false;

}    // ......................................................................................... critical section, end.

void IO::SharedState::Unregister(int fd)  {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................

    // ...

}    // ......................................................................................... critical section, end.


void IO::SharedState::OnCompletion(int completionId) {
    unique_lock<mutex> critical(lock_);    // Critical section..........................................................

    Log::Print(Log::Level::Debug, this, []{ return "worker finished"; });

}    // ......................................................................................... critical section, end.


}    // Namespace IO.
}    // Namespace K.

