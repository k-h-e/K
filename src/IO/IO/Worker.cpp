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
        ClientInfo pipeInfo(pipe_, nullptr);
        bool done = false;
        while (!done) {
            SetUpSelectSets();

            Log::Print(Log::Level::Debug, this, []{ return "sleeping..."; });
            int result = select(highestFileDescriptor_ + 1, &readSet_, &writeSet_, nullptr, nullptr);
            if (result > 0) {
                doIO();
                if (FD_ISSET(pipe_, &readSet_)) {
                    Read(&pipeInfo);
                    if (pipeInfo.error || pipeInfo.eof) {
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
    highestFileDescriptor_ = -1;

    for (auto &pair : clients_) {
        ClientInfo &clientInfo = pair.second;
        if (!clientInfo.error) {
            if (clientInfo.canRead && !clientInfo.eof) {
                FD_SET(clientInfo.fileDescriptor, &readSet_);
            }
            if (clientInfo.canWrite) {
                FD_SET(clientInfo.fileDescriptor, &writeSet_);
            }
            UpdateHighestFileDescriptor(clientInfo.fileDescriptor);
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
        ClientInfo &clientInfo = pair.second;
        if (!clientInfo.error) {
            if (clientInfo.canRead && !clientInfo.eof && FD_ISSET(clientInfo.fileDescriptor, &readSet_)) {
                Read(&clientInfo);
            }
            if (clientInfo.canWrite && FD_ISSET(clientInfo.fileDescriptor, &writeSet_)) {
                Write(&clientInfo);
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
        if (clients_.find(workInfo_.registrationInfo.client) == clients_.end()) {
            clients_[workInfo_.registrationInfo.client]
                 = ClientInfo(workInfo_.registrationInfo.fileDescriptor, workInfo_.registrationInfo.client);
            Log::Print(Log::Level::Debug, this, [&]{
                return "registered client, fd=" + to_string(workInfo_.registrationInfo.fileDescriptor)
                     + ", num=" + to_string(clients_.size());
            });
            success = true;
        }
        sharedState_->OnClientRegistered(success);
    }

    if (workInfo_.clientToUnregister) {
        auto iter = clients_.find(workInfo_.registrationInfo.client);
        if (iter != clients_.end()) {
            int fd = iter->second.fileDescriptor;
            clients_.erase(workInfo_.registrationInfo.client);
            Log::Print(Log::Level::Debug, this, [&]{
                return "unregistered client, fd=" + to_string(fd) + ", num_remaining=" + to_string(clients_.size());
            });
        }
        sharedState_->OnClientUnregistered();
    }

    for (ClientInterface *client : workInfo_.clientsReadyToRead) {
        auto iter = clients_.find(client);
        if (iter != clients_.end()) {
            iter->second.canRead = true;
            Log::Print(Log::Level::Debug, this, [&]{
                return "client can read, fd=" + to_string(iter->second.fileDescriptor); });
        }
    }

    for (ClientInterface *client : workInfo_.clientsReadyToWrite) {
        auto iter = clients_.find(client);
        if (iter != clients_.end()) {
            iter->second.canWrite = true;
            Log::Print(Log::Level::Debug, this, [&]{
                return "client can write, fd=" + to_string(iter->second.fileDescriptor); });
        }
    }

    return true;
}

void IO::Worker::Read(ClientInfo *clientInfo) {
    int numReadTotal = 0;
    while (numReadTotal < bufferSize) {
        int numRead = read(clientInfo->fileDescriptor, buffer_, bufferSize);
        if (numRead > 0) {
            Log::Print(Log::Level::Debug, this, [&]{
                return "fd " + to_string(clientInfo->fileDescriptor)  + " -> " + to_string(numRead) + " bytes";
            });
            numReadTotal += numRead;
            if (clientInfo->client) {
                if (!clientInfo->client->OnDataRead(buffer_, numRead)) {
                    clientInfo->canRead = false;
                    return;
                }
            }
        }
        else if (numRead == -1) {
            if (errno == EINTR) {
                continue;
            }

            if (errno != EAGAIN) {
                clientInfo->error = true;
                if (clientInfo->client) {
                    clientInfo->client->OnError();
                }
            }

            return;
        }
        else {
            clientInfo->eof = true;
            if (clientInfo->client) {
                clientInfo->client->OnEof();
            }
            return;
        }
    }
}

void IO::Worker::Write(ClientInfo *clientInfo) {
    int numWrittenTotal = 0;
    while (numWrittenTotal < bufferSize) {
        int numToWrite = clientInfo->client->OnReadyWrite(buffer_, bufferSize);
        if (!numToWrite) {
            clientInfo->canWrite = false;
            return;
        }
        else {
            const uint8_t *data   = buffer_;
            int           numLeft = numToWrite;
            while (numLeft) {
                int numWritten = write(clientInfo->fileDescriptor, data, numLeft);
                if (numWritten > 0) {
                    Log::Print(Log::Level::Debug, this, [&]{
                        return "fd " + to_string(clientInfo->fileDescriptor)  + " <- " + to_string(numWritten)
                            + " bytes";
                    });
                    numWrittenTotal += numWritten;
                    data    += numWritten;
                    numLeft -= numWritten;
                }
                else if (numWritten == -1) {
                    if (errno == EINTR) {
                        continue;
                    }

                    if (errno == EAGAIN) {
                        clientInfo->client->OnIncompleteWrite(data, numLeft);
                    }
                    else {
                        clientInfo->error = true;
                        if (clientInfo->client) {
                            clientInfo->client->OnError();
                        }
                    }

                    return;
                }
            }
        }
    }
}

}    // Namespace IO.
}    // Namespace K.

