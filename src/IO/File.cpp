#include <K/IO/File.h>

#include <unistd.h>
#include <fcntl.h>
#include <cerrno>
#include <cassert>
#include <K/Core/Result.h>
#include <K/Core/Log.h>
#include <K/IO/IOTools.h>

using std::shared_ptr;
using std::string;
using std::to_string;
using K::Core::Result;
using K::Core::Log;

namespace K {
namespace IO {

File::File(const string &fileName, AccessMode accessMode, bool truncate)
        : File(fileName, accessMode, truncate, nullptr) {
    // Nop.
}

File::File(const string &fileName, AccessMode accessMode, bool truncate, const shared_ptr<Result> &resultAcceptor)
        : fd_(-1),
          position_(0),
          eof_(false),
          error_(true),
          finalResultAcceptor_(resultAcceptor) {
    AccessModeToFlags(accessMode, &readable_, &writable_);

    int flags = 0;
    if (readable_ && !writable_) {
        flags |= O_RDONLY;
    }
    else if (!readable_ && writable_) {
        flags |= O_WRONLY;
    }
    else {
        flags |= O_RDWR;
    }

    if (writable_) {
        flags |= O_CREAT;
        if (truncate) {
            flags |= O_TRUNC;
        }
    }

    bool done = false;
    while (!done) {
        int fd = open(fileName.c_str(), flags, 0600);
        if (fd >= 0) {
            fd_    = fd;
            error_ = false;
            done   = true;
            Log::Print(Log::Level::Debug, this, [&]{
                return string("file \"") + fileName + "\" opened, fd=" + to_string(fd_); });
        }
        else {
            if (errno != EINTR) {
                done = true;
                Log::Print(Log::Level::Warning, this, [&]{
                    return string("failed to open file \"") + fileName + "\""; });
            }
        }
    }
}

File::~File() {
    if (fd_ != -1) {
        if (!IOTools::CloseFileDescriptor(fd_, this)) {
            error_ = true;
        }
    }

    if (finalResultAcceptor_) {
        finalResultAcceptor_->Set(!error_);
    }
}

int File::Read(void *outBuffer, int bufferSize) {
    assert(bufferSize > 0);
    if (!error_) {
        if (readable_) {
            bool done = false;
            while (!done) {
                int numRead = read(fd_, outBuffer, bufferSize);
                if (numRead > 0) {
                    position_ += numRead;
                    return numRead;
                }
                else if (numRead == 0) {
                    eof_ = true;
                    Log::Print(Log::Level::Debug, this, [&]{
                        return "reading on fd " + to_string(fd_) + " reached EOF"; });
                    return 0;
                }
                else {
                    if (errno != EINTR) {
                        done = true;
                    }
                }
            }
        }

        error_ = true;
        Log::Print(Log::Level::Warning, this, [&]{ return "error while reading from fd " + to_string(fd_); });
    }

    return 0;
}

int File::Write(const void *data, int dataSize) {
    assert(dataSize > 0);
    if (!error_) {
        if (writable_) {
            bool done = false;
            while (!done) {
                int numWritten = write(fd_, data, dataSize);
                if (numWritten >= 0) {
                    position_ += numWritten;
                    eof_       = false;
                    return numWritten;
                }
                else {
                    if (errno != EINTR) {
                        done = true;
                    }
                }
            }
        }

        error_ = true;
        Log::Print(Log::Level::Warning, this, [&]{ return "error while writing on fd " + to_string(fd_); });
    }

    return 0;
}

bool File::Seek(int64_t position) {
    assert(position >= 0);
    if (!error_) {
        if (lseek(fd_, static_cast<off_t>(position), SEEK_SET) == static_cast<off_t>(position)) {
            position_ = position;
            eof_      = false;
            return true;
        }

        error_ = true;
        Log::Print(Log::Level::Warning, this, [&]{
            return "error while seeking in fd " + to_string(fd_) + " to position " + to_string(position); });
    }

    return false;
}

int64_t File::StreamPosition() {
    return position_;
}

bool File::Eof() {
    return eof_;
}

bool File::ErrorState() {
    return error_;
}

void File::SetFinalResultAcceptor(const shared_ptr<Result> &resultAcceptor) {
    finalResultAcceptor_ = resultAcceptor;
}

void File::AccessModeToFlags(AccessMode accessMode, bool *outReadable, bool *outWritable) {
    switch (accessMode) {
        case File::AccessMode::ReadOnly:
            *outReadable = true;
            *outWritable = false;
            break;
        case File::AccessMode::WriteOnly:
            *outReadable = false;
            *outWritable = true;
            break;
        case File::AccessMode::ReadWrite:
        default:
            *outReadable = true;
            *outWritable = true;
            break;
    }
}

}    // Namesapce IO.
}    // Namespace K.
