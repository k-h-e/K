/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#include <K/IO/ConnectionIO.h>

#include <unistd.h>
#include <fcntl.h>
#include <K/Core/ThreadPool.h>
#include <K/Core/Log.h>
#include "SharedState.h"
#include "Worker.h"

using std::shared_ptr;
using std::make_shared;
using K::Core::ThreadPool;
using K::Core::Log;

namespace K {
namespace IO {

ConnectionIO::ConnectionIO(const shared_ptr<ThreadPool> &threadPool) {
    int  fileDescriptors[2];
    bool havePipe = false;
    if (!pipe(fileDescriptors)) {
        int flags0 = fcntl(fileDescriptors[0], F_GETFL);
        if (flags0 != -1) {
            if (fcntl(fileDescriptors[0], F_SETFL, flags0 | O_NONBLOCK) != -1) {
                havePipe = true;
            }
        }

        if (!havePipe) {
            close(fileDescriptors[0]);
            close(fileDescriptors[1]);
            Log::Print(Log::Level::Warning, this, []{ return "failed to set pipe end non-blocking"; });
        }
    }
    if (!havePipe) {
        fileDescriptors[0] = -1;
        fileDescriptors[1] = -1;
        Log::Print(Log::Level::Warning, this, []{ return "failed to create pipe"; });
    }

    sharedState_ = make_shared<SharedState>(fileDescriptors[1]);
    worker_      = make_shared<Worker>(fileDescriptors[0], sharedState_);

    threadPool->Run(worker_.get(), sharedState_.get(), 0);
}

ConnectionIO::~ConnectionIO() {
    sharedState_->ShutDown();
}

bool ConnectionIO::Register(const shared_ptr<ClientInterface> &client, int fd) {
    return sharedState_->Register(client, fd);
}

void ConnectionIO::Unregister(const shared_ptr<ClientInterface> &client, bool *outFinalStreamError) {
    sharedState_->Unregister(client, outFinalStreamError);
}

void ConnectionIO::SetClientCanRead(ClientInterface *client) {
    sharedState_->SetClientCanRead(client);
}

void ConnectionIO::SetClientCanWrite(ClientInterface *client) {
    sharedState_->SetClientCanWrite(client);
}

void ConnectionIO::RequestCustomCall(ClientInterface *client) {
    sharedState_->RequestCustomCall(client);
}

void ConnectionIO::RequestErrorState(ClientInterface *client) {
    sharedState_->RequestErrorState(client);
}

}    // Namespace IO.
}    // Namespace K.
