#include <K/IO/BufferedConnection.h>

#include <unistd.h>
#include <cassert>
#include <K/Core/ResultAcceptor.h>
#include <K/Core/Log.h>
#include <K/IO/ConnectionIO.h>
#include <K/IO/IOTools.h>
#include "SharedState.h"

using std::shared_ptr;
using std::make_shared;
using std::optional;
using std::to_string;
using K::Core::Log;
using K::Core::ResultAcceptor;
using K::Core::StreamHandlerInterface;
using K::Core::StreamInterface;

namespace K {
namespace IO {

BufferedConnection::BufferedConnection(
    optional<int> fd, int bufferSizeThreshold, const shared_ptr<ConnectionIO> &connectionIO)
        : connectionIO_(connectionIO),
          fd_(fd) {
    sharedState_ = make_shared<SharedState>(bufferSizeThreshold, connectionIO_);
    if (fd_) {
        if (!connectionIO_->Register(sharedState_, *fd_)) {
            (void)IOTools::CloseFileDescriptor(*fd_, this);
            Log::Print(Log::Level::Warning, this, [&]{
                return "failed to register fd " + to_string(*fd_) + " with central I/O"; });
            fd_.reset();
        }
    }
    else {
        Log::Print(Log::Level::Warning, this, [&]{ return "no file descriptor given"; });
    }

    if (!fd_) {
        sharedState_->SetError();
    }
}

BufferedConnection::~BufferedConnection() {
    bool error;

    // Note: We're currently not waiting for all remaining buffered write data to be written!

    if (fd_) {
        Log::Print(Log::Level::Debug, this, [&]{ return "unregistering fd " + to_string(*fd_) + " from central I/O"; });
        bool finalStreamError = true;
        connectionIO_->Unregister(sharedState_, &finalStreamError);
        Log::Print(Log::Level::Debug, this, [&]{
            return "fd " + to_string(*fd_) + " unregistered from central I/O" + ", final_stream_error="
                + to_string(finalStreamError); });

        bool closeError = !IOTools::CloseFileDescriptor(*fd_, this);

        error = finalStreamError || closeError;
    }
    else {
        error = true;
    }

    if (closeResultAcceptor_) {
        if (error) {
            closeResultAcceptor_->OnFailure();
        } else {
            closeResultAcceptor_->OnSuccess();
        }
    }
}

void BufferedConnection::TriggerErrorState() {
    Log::Print(Log::Level::Debug, this, [&]{ return "TriggerErrorState()"; });
    connectionIO_->RequestErrorState(sharedState_.get());
}

bool BufferedConnection::Register(const shared_ptr<StreamHandlerInterface> &handler, int activationId) {
    return sharedState_->Register(handler, activationId);
}

void BufferedConnection::Unregister(const shared_ptr<StreamHandlerInterface> &handler) {
    sharedState_->Unregister(handler);
}

int BufferedConnection::WriteBlocking(const void *data, int dataSize) {
    assert(dataSize > 0);
    sharedState_->WriteItem(data, dataSize);
    return dataSize;
}

bool BufferedConnection::ErrorState() const {
    return sharedState_->Error();
}

StreamInterface::Error BufferedConnection::StreamError() const {
    return sharedState_->StreamError();
}

void BufferedConnection::SetCloseResultAcceptor(const shared_ptr<ResultAcceptor> &resultAcceptor) {
    closeResultAcceptor_ = resultAcceptor;
}

}    // Namesapce IO.
}    // Namespace K.
