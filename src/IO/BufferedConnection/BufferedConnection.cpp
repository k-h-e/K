#include <K/IO/BufferedConnection.h>

#include <unistd.h>
#include <cassert>
#include <K/Core/Log.h>
#include <K/IO/ConnectionIO.h>
#include "SharedState.h"

using std::shared_ptr;
using std::make_shared;
using std::to_string;
using K::Core::Log;
using K::IO::ConnectionIO;

namespace K {
namespace IO {

BufferedConnection::BufferedConnection(int fd, int bufferSizeThreshold, const shared_ptr<ConnectionIO> &connectionIO)
        : connectionIO_(connectionIO),
          fd_(fd) {
    sharedState_ = make_shared<SharedState>(bufferSizeThreshold, connectionIO_);

    if (!connectionIO_->Register(sharedState_.get(), fd_)) {
        sharedState_->SetError();
    }
}

BufferedConnection::~BufferedConnection() {
    bool finalStreamError = true;
    connectionIO_->Unregister(sharedState_.get(), &finalStreamError);

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

bool BufferedConnection::ErrorState() {
    return sharedState_->Error();
}

}    // Namesapce IO.
}    // Namespace K.
