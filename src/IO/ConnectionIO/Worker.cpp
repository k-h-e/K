/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's C++ Crossplatform Assets                                                                        ///
//    (C) Copyright Kai Hergenröther.                                                                      //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#include "Worker.h"

#include <unistd.h>
#include <signal.h>
#include <K/Core/Log.h>
#include <K/IO/IOTools.h>
#include "SharedState.h"

using std::shared_ptr;
using std::to_string;
using K::Core::Log;
using K::IO::IOTools;

namespace K {
namespace IO {

ConnectionIO::Worker::Worker(int pipe, shared_ptr<SharedState> sharedState)
        : sharedState_(sharedState),
          pipe_(pipe),
          highestFileDescriptor_(-1) {
    // Nop.
}

ConnectionIO::Worker::~Worker() {
    if (pipe_ != -1) {
        (void)IOTools::CloseFileDescriptor(pipe_, this);
    }
}

void ConnectionIO::Worker::ExecuteAction() {
    Log::Print(Log::Level::Debug, this, [&]{ return "spawning, pipe=" + to_string(pipe_) + "..."; });

    if (pipe_ != -1) {
        ClientInfo pipeInfo(pipe_, nullptr);
        if (BlockSigPipe()) {
            bool done = false;
            while (!done) {
                SetUpSelectSets();
                Log::Print(Log::Level::DebugDebug, this, []{ return "sleeping..."; });
                int result = select(highestFileDescriptor_ + 1, &readSet_, &writeSet_, nullptr, nullptr);
                Log::Print(Log::Level::DebugDebug, this, [&]{ return "woken up, select_result=" + to_string(result); });
                if (result > 0) {
                    clientsToUnregister_.clear();
                    DoIO();
                    if (FD_ISSET(pipe_, &readSet_)) {
                        Read(&pipeInfo);
                        if (pipeInfo.error || pipeInfo.eof) {
                            done = true;
                        }
                        else {
                            if (!ProcessClientRequests()) {
                                Log::Print(Log::Level::Debug, this, []{ return "shutdown requested"; });
                                done = true;
                            }
                        }
                    }
                    UnregisterClients();
                }
                else if (result < 0) {
                    if (errno == EINTR) {
                        // Nop.
                    }
                    else {
                        Log::Print(Log::Level::Warning, this, []{ return "select() call failed"; });
                        done = true;
                    }
                }
            }
        }
    }
    else {
        Log::Print(Log::Level::Warning, this, []{ return "bad pipe"; });
    }

    for (auto &pair : clients_) {
        ClientInfo &clientInfo = pair.second;
        if (!clientInfo.error) {
            clientInfo.client->OnError();
        }
    }
    sharedState_->OnErrorState();

    Log::Print(Log::Level::Debug, this, []{ return "terminating"; });
}

bool ConnectionIO::Worker::BlockSigPipe() {
    sigset_t signalsToBlock;
    sigemptyset(&signalsToBlock);
    sigaddset(&signalsToBlock, SIGPIPE);
    if (!pthread_sigmask(SIG_BLOCK, &signalsToBlock, nullptr)) {
        return true;
    }

    Log::Print(Log::Level::Warning, this, []{ return "failed to block SIGPIPE"; });
    return false;
}

void ConnectionIO::Worker::SetUpSelectSets() {
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

void ConnectionIO::Worker::UpdateHighestFileDescriptor(int fileDescriptor) {
    if (fileDescriptor > highestFileDescriptor_) {
        highestFileDescriptor_ = fileDescriptor;
    }
}

void ConnectionIO::Worker::DoIO() {
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

void ConnectionIO::Worker::UnregisterClients() {
    for (ClientInterface *client : clientsToUnregister_) {
        auto iter = clients_.find(client);
        if (iter != clients_.end()) {
            ClientInfo &clientInfo = iter->second;
            int  fd               = clientInfo.fileDescriptor;
            bool finalStreamError = clientInfo.error;
            clients_.erase(client);
            fileDescriptors_.erase(fd);
            sharedState_->OnClientUnregistered(finalStreamError);

            Log::Print(Log::Level::Debug, this, [&]{ return "unregistered client, fd=" + to_string(fd)
                + ", num_remaining=" + to_string(clients_.size());
            });
        }
    }

    clientsToUnregister_.clear();
}

bool ConnectionIO::Worker::ProcessClientRequests() {
    bool shutDownRequested = false;

    sharedState_->GetWork(&workInfo_);
    if (workInfo_.shutDownRequested) {
        shutDownRequested = true;
    }
    else {
        if (workInfo_.registrationInfo.client) {
            bool success = false;
            if (clients_.find(workInfo_.registrationInfo.client.get()) == clients_.end()) {
                if (fileDescriptors_.find(workInfo_.registrationInfo.fileDescriptor) == fileDescriptors_.end()) {
                    clients_[workInfo_.registrationInfo.client.get()]
                         = ClientInfo(workInfo_.registrationInfo.fileDescriptor, workInfo_.registrationInfo.client);
                    fileDescriptors_.insert(workInfo_.registrationInfo.fileDescriptor);
                    Log::Print(Log::Level::Debug, this, [&]{
                        return "registered client, fd=" + to_string(workInfo_.registrationInfo.fileDescriptor)
                             + ", num=" + to_string(clients_.size());
                    });
                    success = true;
                }
                else {
                    Log::Print(Log::Level::Warning, this, [&]{
                        return "fd " + to_string(workInfo_.registrationInfo.fileDescriptor) + " already registered"; });
                }
            }
            else {
                Log::Print(Log::Level::Warning, this, []{ return "client already registered"; });
            }
            sharedState_->OnClientRegistered(success);
        }

        if (workInfo_.unregistrationInfo.client) {
            auto iter = clients_.find(workInfo_.unregistrationInfo.client.get());
            if (iter != clients_.end()) {
                ClientInfo &clientInfo = iter->second;
                clientInfo.unregistering = true;
                clientInfo.canWrite      = true;

                if (clientInfo.error) {
                    ScheduleClientDeregistration(clientInfo);
                }
            }
            else {
                Log::Print(Log::Level::Warning, this, [&]{ return "no such client to unregister"; });
                sharedState_->OnClientUnregistered(true);
            }
        }

        for (ClientInterface *client : workInfo_.clientsReadyToRead) {
            auto iter = clients_.find(client);
            if (iter != clients_.end()) {
                iter->second.canRead = true;
                Log::Print(Log::Level::DebugDebug, this, [&]{
                    return "client can read, fd=" + to_string(iter->second.fileDescriptor); });
            }
        }

        for (ClientInterface *client : workInfo_.clientsReadyToWrite) {
            auto iter = clients_.find(client);
            if (iter != clients_.end()) {
                iter->second.canWrite = true;
                Log::Print(Log::Level::DebugDebug, this, [&]{
                    return "client can write, fd=" + to_string(iter->second.fileDescriptor); });
            }
        }

        for (ClientInterface *client : workInfo_.clientsWithCustomCallRequested) {
            auto iter = clients_.find(client);
            if (iter != clients_.end()) {
                iter->second.client->OnCustomCall();
            }
        }

        for (ClientInterface *client : workInfo_.clientsWithErrorStateRequested) {
            auto iter = clients_.find(client);
            if (iter != clients_.end()) {
                SetClientError(&iter->second);
            }
        }
    }

    workInfo_.Clear();
    return !shutDownRequested;
}

void ConnectionIO::Worker::Read(ClientInfo *clientInfo) {
    int numReadTotal = 0;
    while (numReadTotal < bufferSize) {
        int numRead = read(clientInfo->fileDescriptor, buffer_, bufferSize);
        if (numRead > 0) {
            Log::Print(Log::Level::DebugDebug, this, [&]{
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
                SetClientError(clientInfo);
            }
            return;
        }
        else {
            Log::Print(Log::Level::Debug, this, [&]{ return "EOF on fd " + to_string(clientInfo->fileDescriptor); });
            clientInfo->eof = true;
            if (clientInfo->client) {
                clientInfo->client->OnEof();
            }
            return;
        }
    }
}

void ConnectionIO::Worker::Write(ClientInfo *clientInfo) {
    int numWrittenTotal = 0;
    while (numWrittenTotal < bufferSize) {
        int numToWrite = clientInfo->client->OnReadyWrite(buffer_, bufferSize);
        if (!numToWrite) {
            clientInfo->canWrite = false;
            if (clientInfo->unregistering) {
                ScheduleClientDeregistration(*clientInfo);
            }
            return;
        }
        else {
            const uint8_t *data   = buffer_;
            int           numLeft = numToWrite;
            while (numLeft) {
                int numWritten = write(clientInfo->fileDescriptor, data, numLeft);
                if (numWritten > 0) {
                    Log::Print(Log::Level::DebugDebug, this, [&]{
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
                        Log::Print(Log::Level::Debug, this, [&]{
                            return "incomplete write on fd " + to_string(clientInfo->fileDescriptor)
                                + ", put back " + to_string(numLeft) + " bytes";
                        });

                        clientInfo->client->OnIncompleteWrite(data, numLeft);
                    }
                    else {
                        SetClientError(clientInfo);
                    }

                    return;
                }
            }
        }
    }
}

void ConnectionIO::Worker::SetClientError(ClientInfo *clientInfo) {
    if (!clientInfo->error) {
        Log::Print(Log::Level::Warning, this, [&]{
            return "error state entered for fd " + to_string(clientInfo->fileDescriptor);
        });

        clientInfo->error = true;
        if (clientInfo->client) {
            clientInfo->client->OnError();
        }

        if (clientInfo->unregistering) {
            ScheduleClientDeregistration(*clientInfo);
        }
    }
}

void ConnectionIO::Worker::ScheduleClientDeregistration(const ClientInfo &clientInfo) {
    clientsToUnregister_.push_back(clientInfo.client.get());
}

}    // Namespace IO.
}    // Namespace K.

