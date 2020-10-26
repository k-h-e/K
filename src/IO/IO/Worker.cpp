#include "Worker.h"

#include <unistd.h>
#include <sys/select.h>
#include <K/Core/Log.h>
#include "SharedState.h"
#include "WorkInfo.h"

using std::shared_ptr;
using std::to_string;
using K::Core::Log;

namespace K {
namespace IO {

IO::Worker::Worker(int pipe, shared_ptr<SharedState> sharedState)
        : sharedState_(sharedState),
          pipe_(pipe),
          highestFileDescriptor_(-1) {
    // Nop.
}

IO::Worker::~Worker() {
    if (pipe_ != -1) {
        close (pipe_);
    }
}

void IO::Worker::ExecuteAction() {
    Log::Print(Log::Level::Debug, this, [&]{ return "spawning, pipe=" + to_string(pipe_) + "..."; });

    if (pipe_ != -1) {
        fd_set readSet;
        fd_set writeSet;
        fd_set errorSet;
        bool done = false;
        while (!done) {
            FD_ZERO(&readSet);
            FD_ZERO(&writeSet);
            FD_ZERO(&errorSet);
            highestFileDescriptor_ = -1;

            FD_SET(pipe_, &readSet);
            UpdateHighestFileDescriptor(pipe_);

            Log::Print(Log::Level::Debug, this, []{ return "sleeping..."; });
            int result = select(highestFileDescriptor_ + 1, &readSet, &writeSet, &errorSet, nullptr);
            Log::Print(Log::Level::Debug, this, []{ return "awake"; });
            if (result > 0) {
                if (FD_ISSET(pipe_, &readSet)) {
                    bool eof;
                    if (!Read(pipe_, &eof) || eof) {
                        sharedState_->SetErrorState();
                        done = true;
                    }
                    else {
                        if (!ProcessClientRequests()) {
                            Log::Print(Log::Level::Debug, this, []{ return "shutdown requested"; });
                            done = true;
                        }
                    }
                }
            }
            else if (result < 0) {
                if (errno == EINTR) {
                    // Nop.
                }
                else {
                    sharedState_->SetErrorState();
                    done = true;
                }
            }
        }
    }

    Log::Print(Log::Level::Debug, this, []{ return "terminating"; });
}

void IO::Worker::UpdateHighestFileDescriptor(int fileDescriptor) {
    if (fileDescriptor > highestFileDescriptor_) {
        highestFileDescriptor_ = fileDescriptor;
    }
}

bool IO::Worker::ProcessClientRequests() {
    WorkInfo info;
    sharedState_->GetWork(&info);
    if (info.shutDownRequested) {
        return false;
    }

    if (info.registrationInfo.fileDescriptor >= 0) {
        bool success = false;
        if (fileDescriptors_.find(info.registrationInfo.fileDescriptor) == fileDescriptors_.end()) {
            fileDescriptors_[info.registrationInfo.fileDescriptor]
                = FileDescriptorInfo(info.registrationInfo.reader, info.registrationInfo.writer);
            Log::Print(Log::Level::Debug, this, [&]{
                return "registered client, fd=" + to_string(info.registrationInfo.fileDescriptor)
                     + ", num=" + to_string(fileDescriptors_.size());
            });
            success = true;
        }
        sharedState_->OnClientRegistered(success);
    }

    if (info.fileDescriptorToUnregister >= 0) {
        if (fileDescriptors_.erase(info.fileDescriptorToUnregister) == 1u) {
            Log::Print(Log::Level::Debug, this, [&]{
                return "unregistered client, fd=" + to_string(info.fileDescriptorToUnregister)
                    + ", num_remaining=" + to_string(fileDescriptors_.size());
            });
        }
        sharedState_->OnClientUnregistered();
    }

    return true;
}

bool IO::Worker::Read(int fileDescriptor, bool *outEof) {
    *outEof = false;

    int numTotal = 0;
    while (true) {
        int numRead = read(fileDescriptor, buffer_, bufferSize);
        if (numRead > 0) {
            numTotal += numRead;
        }
        else if (numRead == -1) {
            if (errno == EAGAIN) {
                if (numTotal > 0) {
                    Log::Print(Log::Level::Debug, this, [&]{
                        return "read " + to_string(numTotal) + " bytes, fd=" + to_string(fileDescriptor);
                    });
                }
                return true;
            }
            else {
                return false;
            }
        }
        else {
            *outEof = true;
            return true;
        }
    }
}

}    // Namespace IO.
}    // Namespace K.

