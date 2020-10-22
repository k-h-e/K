#include <K/IO/IO.h>

#include <unistd.h>
#include <fcntl.h>
#include <K/Core/ThreadPool.h>
#include "SharedState.h"
#include "Worker.h"

using std::shared_ptr;
using std::make_shared;
using K::Core::ThreadPool;

namespace K {
namespace IO {

IO::IO(const shared_ptr<ThreadPool> &threadPool) {
    int  fileDescriptors[2];
    bool havePipe = false;
    if (!pipe(fileDescriptors)) {
        int flags0 = fcntl(fileDescriptors[0], F_GETFL);
        if (flags0 != -1) {
            if (fcntl(fileDescriptors[0], F_SETFL, flags0 | O_NONBLOCK) != -1) {
                havePipe = true;
            }
        }

        if (!havePipe) {
            close(fileDescriptors[0]);
            close(fileDescriptors[1]);
        }
    }
    if (!havePipe) {
        fileDescriptors[0] = -1;
        fileDescriptors[1] = -1;
    }

    sharedState_ = make_shared<SharedState>(fileDescriptors[1]);
    worker_      = make_shared<Worker>(fileDescriptors[0], sharedState_);

    threadPool->Run(worker_, sharedState_, 0);
}

IO::~IO() {
    sharedState_->ShutDown();
}

bool IO::Register(int fd, ReadHandlerInterface *reader, WriteHandlerInterface *writer) {
    return sharedState_->Register(fd, reader, writer);
}

void IO::Unregister(int fd) {
    sharedState_->Unregister(fd);
}

}    // Namespace IO.
}    // Namespace K.
