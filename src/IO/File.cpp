#include <K/IO/File.h>

#include <unistd.h>
#include <fcntl.h>
#include <cstdio>
#include <cerrno>
#include <cassert>
#include <K/Core/ResultAcceptor.h>
#include <K/Core/Log.h>
#include <K/IO/IOTools.h>

using std::shared_ptr;
using std::string;
using std::to_string;
using K::Core::Log;
using K::Core::ResultAcceptor;
using K::Core::StreamInterface;

namespace K {
namespace IO {

File::File(const string &fileName, AccessMode accessMode, bool truncate)
        : fileName_(fileName),
          fd_(-1),
          position_(0),
          error_(Error::Unspecific) {
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
            error_ = Error::None;
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
            if (error_ == Error::None) {
                error_ = Error::IO;
            }
        }
    }

    if (closeResultAcceptor_) {
        if (error_ != Error::None) {
            closeResultAcceptor_->OnFailure();
        } else {
            closeResultAcceptor_->OnSuccess();
        }
    }

    if (error_ != Error::None) {
        Log::Print(Log::Level::Error, this, [&]{
            return "failed to properly close file \"" + fileName_ + "\", fd=" + to_string(fd_);
        });
    } else {
        Log::Print(Log::Level::Debug, this, [&]{
            return "closed file \"" + fileName_ + "\", fd=" + to_string(fd_);
        });
    }
}

int File::ReadBlocking(void *buffer, int bufferSize) {
    assert(bufferSize > 0);
    if (!ErrorState()) {
        if (!readable_) {
            error_ = Error::User;
        } else {
            while (true) {
                int numRead = read(fd_, buffer, bufferSize);
                if (numRead > 0) {
                    position_ += numRead;
                    return numRead;
                } else if (numRead == 0) {
                    error_ = Error::Eof;
                    Log::Print(Log::Level::Debug, this, [&]{
                        return "EOF encountered, file=\"" + fileName_ + "\", fd=" + to_string(fd_);
                    });
                    return 0;
                } else {
                    if (errno != EINTR) {
                        error_ = Error::IO;
                        Log::Print(Log::Level::Error, this, [&]{
                            return "read error, file=\"" + fileName_ + "\", fd=" + to_string(fd_);
                        });
                        return 0;
                    }
                }
            }
        }
    }

    return 0;
}

int File::WriteBlocking(const void *data, int dataSize) {
    assert(dataSize > 0);
    if (!ErrorState()) {
        if (!writable_) {
            error_ = Error::User;
        } else {
            while (true) {
                int numWritten = write(fd_, data, dataSize);
                if (numWritten > 0) {
                    position_ += numWritten;
                    return numWritten;
                } else if (numWritten == 0) {
                    // Nop.
                } else {
                    if (errno != EINTR) {
                        error_ = Error::IO;
                        Log::Print(Log::Level::Error, this, [&]{
                            return "write error, file=\"" + fileName_ + "\", fd=" + to_string(fd_);
                        });
                        return 0;
                    }
                }
            }
        }
    }

    return 0;
}

void File::Seek(int64_t position) {
    if (!ErrorState()) {
        if (!(   (position >= 0)
              && (lseek(fd_, static_cast<off_t>(position), SEEK_SET) == static_cast<off_t>(position)))) {
            error_ = Error::IO;
            Log::Print(Log::Level::Error, this, [&]{
                return "seek failed, file=\"" + fileName_ + "\", fd=" + to_string(fd_) + ", position="
                    + to_string(position);
            });
        }
    }
}

void File::RecoverAndSeek(int64_t position) {
    if (error_ == Error::Eof) {
        error_ = Error::None;
    }
    Seek(position);
}

int64_t File::StreamPosition() const {
    return position_;
}

bool File::ErrorState() const {
    return (error_ != Error::None);
}

StreamInterface::Error File::StreamError() const {
    return error_;
}

void File::SetCloseResultAcceptor(const shared_ptr<ResultAcceptor> &resultAcceptor) {
    closeResultAcceptor_ = resultAcceptor;
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
