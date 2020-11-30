#include "Worker.h"

#include <unistd.h>
#include <K/Core/Log.h>
#include "SharedState.h"

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
        bool done = false;
        while (!done) {
            SetUpSelectSets();

            Log::Print(Log::Level::Debug, this, []{ return "sleeping..."; });
            int result = select(highestFileDescriptor_ + 1, &readSet_, &writeSet_, &errorSet_, nullptr);
            if (result > 0) {
                doIO();
                if (FD_ISSET(pipe_, &readSet_)) {
                    bool eof;
                    bool clientStalling;
                    if (!Read(pipe_, nullptr, &eof, &clientStalling) || eof) {
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

void IO::Worker::SetUpSelectSets() {
    FD_ZERO(&readSet_);
    FD_ZERO(&writeSet_);
    FD_ZERO(&errorSet_);
    highestFileDescriptor_ = -1;

    for (auto &pair : clients_) {
        if (!pair.second.error) {
            FD_SET(pair.first, &errorSet_);
            if (pair.second.canRead && !pair.second.eof) {
                FD_SET(pair.first, &readSet_);
            }
            if (pair.second.canWrite) {
                FD_SET(pair.first, &writeSet_);
            }
            UpdateHighestFileDescriptor(pair.first);
        }
    }

    FD_SET(pipe_, &readSet_);
    UpdateHighestFileDescriptor(pipe_);
}

void IO::Worker::UpdateHighestFileDescriptor(int fileDescriptor) {
    if (fileDescriptor > highestFileDescriptor_) {
        highestFileDescriptor_ = fileDescriptor;
    }
}

void IO::Worker::doIO() {
    for (auto &pair : clients_) {
        if (!pair.second.error) {
            if (FD_ISSET(pair.first, &errorSet_)) {
                pair.second.error = true;
            }
            else {
                if (pair.second.canRead && !pair.second.eof && FD_ISSET(pair.first, &readSet_)) {
                    bool eof;
                    bool clientStalling;
                    if (!Read(pair.first, pair.second.client, &eof, &clientStalling)) {
                        pair.second.error = true;
                    }
                    else {
                        if (clientStalling) {
                            pair.second.canRead = false;
                        }
                        if (eof) {
                            pair.second.eof = true;
                            pair.second.client->OnEof();
                        }
                    }
                }

                if (pair.second.canWrite && FD_ISSET(pair.first, &writeSet_)) {

                }
            }

            if (pair.second.error) {
                pair.second.client->OnError();
            }
        }
    }
}

bool IO::Worker::ProcessClientRequests() {
    sharedState_->GetWork(&workInfo_);
    if (workInfo_.shutDownRequested) {
        return false;
    }

    if (workInfo_.registrationInfo.fileDescriptor >= 0) {
        bool success = false;
        if (clients_.find(workInfo_.registrationInfo.fileDescriptor) == clients_.end()) {
            clients_[workInfo_.registrationInfo.fileDescriptor] = ClientInfo(workInfo_.registrationInfo.client);
            Log::Print(Log::Level::Debug, this, [&]{
                return "registered client, fd=" + to_string(workInfo_.registrationInfo.fileDescriptor)
                     + ", num=" + to_string(clients_.size());
            });
            success = true;
        }
        sharedState_->OnClientRegistered(success);
    }

    if (workInfo_.fileDescriptorToUnregister >= 0) {
        if (clients_.erase(workInfo_.fileDescriptorToUnregister) == 1u) {
            Log::Print(Log::Level::Debug, this, [&]{
                return "unregistered client, fd=" + to_string(workInfo_.fileDescriptorToUnregister)
                    + ", num_remaining=" + to_string(clients_.size());
            });
        }
        sharedState_->OnClientUnregistered();
    }

    for (int fd : workInfo_.clientsReadyToRead) {
        auto iter = clients_.find(fd);
        if (iter != clients_.end()) {
            iter->second.canRead = true;
            Log::Print(Log::Level::Debug, this, [&]{ return "client can read, fd=" + to_string(fd); });
        }
    }

    for (int fd : workInfo_.clientsReadyToWrite) {
        auto iter = clients_.find(fd);
        if (iter != clients_.end()) {
            iter->second.canWrite = true;
            Log::Print(Log::Level::Debug, this, [&]{ return "client can write, fd=" + to_string(fd); });
        }
    }

    return true;
}

bool IO::Worker::Read(int fileDescriptor, ClientInterface *client, bool *outEof, bool *outClientStalling) {
    *outEof            = false;
    *outClientStalling = false;

    while (true) {
        int numRead = read(fileDescriptor, buffer_, bufferSize);
        if (numRead > 0) {
            Log::Print(Log::Level::Debug, this, [&]{
                return "read " + to_string(numRead) + " bytes from fd " + to_string(fileDescriptor);
            });
            if (client) {
                if (!client->OnDataRead(buffer_, numRead)) {
                    *outClientStalling = true;
                    return true;
                }
            }
        }
        else if (numRead == -1) {
            if (errno == EAGAIN) {
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

