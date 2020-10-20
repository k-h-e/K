#include <K/IO/IO.h>

#include <K/Core/ThreadPool.h>
#include "SharedState.h"
#include "Worker.h"

using std::shared_ptr;
using std::make_shared;
using K::Core::ThreadPool;

namespace K {
namespace IO {

IO::IO(const shared_ptr<ThreadPool> &threadPool) {
    sharedState_ = make_shared<SharedState>();
    worker_      = make_shared<Worker>(sharedState_);

    threadPool->Run(worker_, sharedState_, 0);
}

bool IO::Register(int fd, ReadHandlerInterface *reader, WriteHandlerInterface *writer) {
    return sharedState_->Register(fd, reader, writer);
}

void IO::Unregister(int fd) {
    sharedState_->Unregister(fd);
}

}    // Namespace IO.
}    // Namespace K.
