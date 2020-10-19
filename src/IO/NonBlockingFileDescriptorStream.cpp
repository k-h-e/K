#include <K/IO/NonBlockingFileDescriptorStream.h>

namespace K {
namespace IO {

NonBlockingFileDescriptorStream::NonBlockingFileDescriptorStream(int fd)
        : fd_(fd) {
    // Nop.
}

NonBlockingFileDescriptorStream::~NonBlockingFileDescriptorStream() {
    // Nop.
}

int NonBlockingFileDescriptorStream::Read(void *outBuffer, int bufferSize) {
    return 0;
}

int NonBlockingFileDescriptorStream::Write(const void *data, int dataSize) {
    return 0;
}

bool NonBlockingFileDescriptorStream::EndOfStream() {
    return false;
}

bool NonBlockingFileDescriptorStream::Error() {
    return false;
}

void NonBlockingFileDescriptorStream::RegisterListener(Listener *listener) {

}

}    // Namesapce IO.
}    // Namespace K.
