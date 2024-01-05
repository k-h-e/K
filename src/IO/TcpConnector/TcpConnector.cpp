/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#include <K/IO/TcpConnector.h>

#include "SynchronizedState.h"

using std::make_unique;
using std::optional;
using std::shared_ptr;
using std::string;
using K::Core::RunLoop;
using K::Core::ThreadPool;

namespace K {
namespace IO {

TcpConnector::TcpConnector(const string &hostAndPort, IO::Deprecated::TcpConnector::HandlerInterface *handler,
                           const shared_ptr<RunLoop> &runLoop, const shared_ptr<ThreadPool> &threadPool)
        : runLoop_{runLoop},
          handler_{handler},
          finished_{false} {
    runLoopClientId_   = runLoop_->AddClient(this);
    synchronizedState_ = make_unique<SynchronizedState>(runLoop_, runLoopClientId_);
    connector_         = make_unique<IO::Deprecated::TcpConnector>(hostAndPort, synchronizedState_.get(), threadPool);
}

TcpConnector::~TcpConnector() {
    connector_.reset();
    runLoop_->RemoveClient(runLoopClientId_);
}

void TcpConnector::Activate(bool deepActivation) {
    (void)deepActivation;
    if (!finished_) {
        optional<int> fd;
        synchronizedState_->Sync(&fd, &finished_);
        if (finished_) {
            if (fd) {
                handler_->OnTcpConnectionEstablished(*fd);
            } else {
                handler_->OnFailedToEstablishTcpConnection();
            }
        }
    }
}

}    // Namespace IO.
}    // Namespace K.
