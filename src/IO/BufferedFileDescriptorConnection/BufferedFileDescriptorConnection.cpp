#include <K/IO/BufferedFileDescriptorConnection.h>

#include <cassert>
#include <K/Core/Log.h>
#include <K/IO/IO.h>
#include "SharedState.h"

using std::shared_ptr;
using std::make_shared;
using K::IO::IO;

namespace K {
namespace IO {

BufferedFileDescriptorConnection::BufferedFileDescriptorConnection(int fd, int bufferSizeThreshold,
                                                                   const shared_ptr<IO> &io) {
    sharedState_ = make_shared<SharedState>(fd, bufferSizeThreshold, io);
}

void BufferedFileDescriptorConnection::Register(HandlerInterface *handler) {
    sharedState_->Register(handler);
}

bool BufferedFileDescriptorConnection::WriteItem(const void *item, int itemSize) {
    return sharedState_->WriteItem(item, itemSize);
}

bool BufferedFileDescriptorConnection::Eof() {
    return sharedState_->Eof();
}

bool BufferedFileDescriptorConnection::Error() {
    return sharedState_->Error();
}

}    // Namesapce IO.
}    // Namespace K.
