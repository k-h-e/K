#include <K/IO/File.h>

#include <unistd.h>
#include <fcntl.h>
#include <cerrno>
#include <cassert>
#include <K/Core/Log.h>
#include <K/IO/IOTools.h>

using std::string;
using std::to_string;
using K::Core::Log;

namespace K {
namespace IO {

File::File(const string &fileName, bool truncate)
        : fd_(-1),
          position_(0),
          eof_(false),
          error_(true) {
    int flags = O_RDWR | O_CREAT;
    if (truncate) {
        flags |= O_TRUNC;
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
        IOTools::CloseFileDescriptor(fd_, this);
    }
}

int File::Read(void *outBuffer, int bufferSize) {
    assert(bufferSize > 0);
    int numRead = 0;
    if (!error_) {
        bool done = false;
        while (!done) {
            numRead = read(fd_, outBuffer, bufferSize);
            if (numRead > 0) {
                position_ += numRead;
                done       = true;
            }
            else if (numRead == 0) {
                eof_ = true;
                done = true;
                Log::Print(Log::Level::Debug, this, [&]{ return "reading on fd " + to_string(fd_) + " reached EOF"; });
            }
            else {
                if (errno != EINTR) {
                    numRead = 0;
                    error_  = true;
                    done    = true;
                    Log::Print(Log::Level::Warning, this, [&]{
                        return "error encountered while reading from fd " + to_string(fd_); });
                }
            }
        }
    }

    return numRead;
}

int File::Write(const void *data, int dataSize) {
    assert(dataSize > 0);
    int numWritten = 0;
    if (!error_) {
        bool done = false;
        while (!done) {
            numWritten = write(fd_, data, dataSize);
            if (numWritten >= 0) {
                position_ += numWritten;
                eof_       = false;
                done       = true;
            }
            else {
                if (errno != EINTR) {
                    numWritten = 0;
                    error_     = true;
                    done       = true;
                    Log::Print(Log::Level::Warning, this, [&]{
                        return "error encountered while writing on fd " + to_string(fd_); });
                }
            }
        }
    }

    return numWritten;
}

bool File::Seek(int64_t position) {
    assert(position >= 0);
    bool success = false;
    if (!error_) {
        if (lseek(fd_, static_cast<off_t>(position), SEEK_SET) == static_cast<off_t>(position)) {
            position_ = position;
            eof_      = false;
            success   = true;
        }
        else {
            error_ = true;
            Log::Print(Log::Level::Warning, this, [&]{
                return "error encountered while seeking in fd " + to_string(fd_); });
        }
    }

    return success;
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

}    // Namesapce IO.
}    // Namespace K.
