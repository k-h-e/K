#include <K/IO/File.h>

namespace K {
namespace IO {

File::File()
        : fd_(-1),
          eof_(false),
          error_(false) {
    // Nop.
}

File::~File() {

}

int File::Read(void *outBuffer, int bufferSize) {
    return 0;
}

int File::Write(const void *data, int dataSize) {
    return 0;
}

bool File::Seek(int64_t position) {
    return false;
}

int64_t File::StreamPosition() {
    return 0u;
}

bool File::Eof() {
    return eof_;
}

bool File::ErrorState() {
    return error_;
}

}    // Namesapce IO.
}    // Namespace K.
