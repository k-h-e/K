#include <K/IO/BufferedConnection.h>

#include <unistd.h>
#include <cassert>
#include <K/Core/Log.h>
#include <K/IO/ConnectionIO.h>
#include <K/IO/IOTools.h>
#include "SharedState.h"

using std::shared_ptr;
using std::make_shared;
using std::to_string;
using K::Core::Log;

namespace K {
namespace IO {

BufferedConnection::BufferedConnection(int fd, int bufferSizeThreshold, const shared_ptr<ConnectionIO> &connectionIO)
        : connectionIO_(connectionIO),
          fd_(fd) {
    assert(fd >= 0);
    sharedState_ = make_shared<SharedState>(bufferSizeThreshold, connectionIO_);

    if (!connectionIO_->Register(sharedState_.get(), fd_)) {
        Log::Print(Log::Level::Error, this, [&]{
            return "failed to register fd " + to_string(fd_) + " with central I/O"; });
        sharedState_->SetError();
    }
}

BufferedConnection::~BufferedConnection() {
    Log::Print(Log::Level::Debug, this, [&]{ return "unregistering fd " + to_string(fd_) + " from central I/O"; });
    bool finalStreamError = true;
    connectionIO_->Unregister(sharedState_.get(), &finalStreamError);
    Log::Print(Log::Level::Debug, this, [&]{
        return "fd " + to_string(fd_) + " unregistered from central I/O" + ", final_stream_error="
            + to_string(finalStreamError); });

    bool closeError = !IOTools::CloseFileDescriptor(fd_, this);

    bool error = finalStreamError || closeError;
    Log::Print(Log::Level::Debug, this, [&]{ return "stream for fd " + to_string(fd_) + " closed, error="
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
