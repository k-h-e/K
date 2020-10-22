#include "Worker.h"

#include <unistd.h>
#include <sys/select.h>
#include <K/Core/Log.h>
#include "SharedState.h"

using std::shared_ptr;
using std::to_string;
using K::Core::Log;

namespace K {
namespace IO {

IO::Worker::Worker(int pipe, shared_ptr<SharedState> sharedState)
        : sharedState_(sharedState),
          pipe_(pipe) {
    // Nop.
}

IO::Worker::~Worker() {
    if (pipe_ != -1) {
        close (pipe_);
    }
}

void IO::Worker::ExecuteAction() {
    Log::Print(Log::Level::Debug, this, [&]{ return "spawning, pipe=" + to_string(pipe_) + "..."; });

    const int bufferSize = 8192;
    uint8_t buffer[bufferSize];

    if (pipe_ != -1) {
        fd_set readSet;
        fd_set writeSet;
        fd_set errorSet;
        bool done = false;
        while (!done) {
            FD_ZERO(&readSet);
            FD_ZERO(&writeSet);
            FD_ZERO(&errorSet);
            highFD_ = -1;

            FD_SET(pipe_, &readSet);
            UpdateHighFD(pipe_);

            Log::Print(Log::Level::Debug, this, []{ return "select()..."; });
            int result = select(highFD_ + 1, &readSet, &writeSet, &errorSet, nullptr);
            Log::Print(Log::Level::Debug, this, [&]{ return "    select() returned, result=" + to_string(result); });
            if (result > 0) {
                if (FD_ISSET(pipe_, &readSet)) {
                    int num = 0;
                    while (!done) {
                        int numRead = read(pipe_, buffer, bufferSize);
                        if (numRead > 0) {
                            num += numRead;
                        }
                        else if (numRead == -1) {
                            if (errno == EAGAIN) {
                                if (num > 0) {
                                    Log::Print(Log::Level::Debug, this, [&]{
                                        return "read " + to_string(num) + " bytes from pipe";
                                    });
                                }
                                break;
                            }
                            else {
                                sharedState_->SetErrorState();
                                done = true;
                            }
                        }
                        else {
                            sharedState_->SetErrorState();
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

void IO::Worker::UpdateHighFD(int fd) {
    if (fd > highFD_) {
        highFD_ = fd;
    }
}

}    // Namespace IO.
}    // Namespace K.

