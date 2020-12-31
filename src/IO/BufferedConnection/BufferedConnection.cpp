#include <K/IO/BufferedConnection.h>

#include <unistd.h>
#include <cassert>
#include <K/Core/Log.h>
#include <K/IO/IO.h>
#include "SharedState.h"

using std::shared_ptr;
using std::make_shared;
using std::to_string;
using K::Core::Log;
using K::IO::IO;

namespace K {
namespace IO {

BufferedConnection::BufferedConnection(int fd, int bufferSizeThreshold, const shared_ptr<IO> &io)
        : io_(io),
          fd_(fd) {
    sharedState_ = make_shared<SharedState>(bufferSizeThreshold, io_);

    if (!io_->Register(sharedState_.get(), fd_)) {
        sharedState_->SetError();
    }
}

BufferedConnection::~BufferedConnection() {
    bool finalStreamError = true;
    io_->Unregister(sharedState_.get(), &finalStreamError);

    bool closedSuccessfully = false;
    bool closeError         = false;
    while (!closedSuccessfully && !closeError) {
        if (!close(fd_)) {
            closedSuccessfully = true;
        }
        else {
            if (errno != EINTR) {
                closeError = true;
            }
        }
    }

    bool error = finalStreamError || closeError;
    Log::Print(Log::Level::Debug, this, [&]{ return "fd " + to_string(fd_) + " closed, error="
        + to_string(error); });
}

void BufferedConnection::Register(HandlerInterface *handler) {
    sharedState_->Register(handler);
}

bool BufferedConnection::WriteItem(const void *item, int itemSize) {
    return sharedState_->WriteItem(item, itemSize);
}

bool BufferedConnection::Eof() {
    return sharedState_->Eof();
}

bool BufferedConnection::Error() {
    return sharedState_->Error();
}

}    // Namesapce IO.
}    // Namespace K.
