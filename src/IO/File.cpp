#include <K/IO/File.h>

#include <unistd.h>
#include <fcntl.h>
#include <cstdio>
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
        : fileName_(fileName),
          fd_(-1),
          position_(0),
          readFailed_(false),
          writeFailed_(false),
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
                return string("opened file \"") + fileName + "\", fd=" + to_string(fd_); });
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

    Result finalResult(!error_);
    if (finalResultAcceptor_) {
        *finalResultAcceptor_ = finalResult;
    }
    Log::Print(Log::Level::Debug, this, [&]{
        return "closed file \"" + fileName_ + "\", fd=" + to_string(fd_) + ", final_result=" + finalResult.ToString();
    });
}

int File::ReadBlocking(void *buffer, int bufferSize) {
    assert(bufferSize > 0);
    int numRead = 0;
    if (!readFailed_) {
        if (!error_ && !eof_) {
            if (!readable_) {
                error_ = true;
            } else {
                bool done = false;
                while (!done) {
                    numRead = read(fd_, buffer, bufferSize);
                    if (numRead > 0) {
                        position_ += numRead;
                        done = true;
                    } else if (numRead == 0) {
                        eof_ = true;
                        done = true;
                        Log::Print(Log::Level::Debug, this, [&]{
                            return "EOF encountered, file=\"" + fileName_ + "\", fd=" + to_string(fd_);
                        });
                    } else {
                        if (errno != EINTR) {
                            error_ = true;
                            done   = true;
                            Log::Print(Log::Level::Error, this, [&]{
                                return "read error, file=\"" + fileName_ + "\", fd=" + to_string(fd_);
                            });
                        }
                    }
                }
            }
        }
    }

    if (!numRead) {
        readFailed_ = true;
    }
    return numRead;
}

bool File::ReadFailed() const {
    return readFailed_;
}

void File::ClearReadFailed() {
    readFailed_ = false;
}

int File::WriteBlocking(const void *data, int dataSize) {
    assert(dataSize > 0);
    int numWritten = 0;
    if (!writeFailed_) {
        if (!error_) {
            if (!writable_) {
                error_ = true;
            } else {
                bool done = false;
                while (!done) {
                    numWritten = write(fd_, data, dataSize);
                    if (numWritten >= 0) {
                        position_ += numWritten;
                        done = true;
                    } else {
                        if (errno != EINTR) {
                            error_ = true;
                            done = true;
                            Log::Print(Log::Level::Error, this, [&]{
                                return "write error, file=\"" + fileName_ + "\", fd=" + to_string(fd_);
                            });
                        }
                    }
                }
            }
        }
    }

    if (!numWritten) {
        writeFailed_ = true;
    }
    return numWritten;
}

bool File::WriteFailed() const {
    return writeFailed_;
}

void File::ClearWriteFailed() {
    writeFailed_ = false;
}

void File::Seek(int64_t position) {
    if (!error_) {
        if ((position >= 0) && (lseek(fd_, static_cast<off_t>(position), SEEK_SET) == static_cast<off_t>(position))) {
            eof_ = false;
        } else {
            error_ = true;
            Log::Print(Log::Level::Error, this, [&]{
                return "seek failed, file=\"" + fileName_ + "\", fd=" + to_string(fd_) + ", position="
                    + to_string(position);
            });
        }
    }
}

int64_t File::StreamPosition() const {
    return position_;
}

bool File::Eof() const {
    return (eof_ && !error_);
}

bool File::ErrorState() const {
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

bool File::Rename(const std::string &oldFileName, const std::string &newFileName) {
    return (std::rename(oldFileName.c_str(), newFileName.c_str()) == 0);
}

}    // Namesapce IO.
}    // Namespace K.
