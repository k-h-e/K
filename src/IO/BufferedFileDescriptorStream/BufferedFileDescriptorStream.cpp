#include <K/IO/BufferedFileDescriptorStream.h>

#include <cassert>
#include <K/Core/Log.h>
#include <K/IO/IO.h>
#include "SharedState.h"

using std::shared_ptr;
using std::make_shared;
using K::IO::IO;

namespace K {
namespace IO {

BufferedFileDescriptorStream::BufferedFileDescriptorStream(int fd, int bufferSizeThreshold, const shared_ptr<IO> &io) {
    sharedState_ = make_shared<SharedState>(fd, bufferSizeThreshold, io);
}

void BufferedFileDescriptorStream::Register(HandlerInterface *handler) {
    sharedState_->Register(handler);
}

int BufferedFileDescriptorStream::Read(void *outBuffer, int bufferSize) {
    return sharedState_->Read(outBuffer, bufferSize);
}

int BufferedFileDescriptorStream::Write(const void *data, int dataSize) {
    return sharedState_->Write(data, dataSize);
}

bool BufferedFileDescriptorStream::Eof() {
    return sharedState_->Eof();
}

bool BufferedFileDescriptorStream::Error() {
    return sharedState_->Error();
}

}    // Namesapce IO.
}    // Namespace K.
