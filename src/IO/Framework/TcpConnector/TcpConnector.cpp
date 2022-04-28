////    ////
////   ////     K Crossplatform C++ Assets
////  ////      (C) Copyright Kai Hergenröther
//// ////
////////        - IO / Framework -
//// ////
////  ////
////   ////
////    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <K/IO/Framework/TcpConnector.h>

#include "SynchronizedState.h"

using std::make_unique;
using std::optional;
using std::shared_ptr;
using std::string;
using K::Core::ThreadPool;
using K::Core::Framework::RunLoop;

namespace K {
namespace IO {
namespace Framework {

TcpConnector::TcpConnector(const string &hostAndPort, IO::TcpConnector::HandlerInterface *handler,
                           int handlerActivationId, const shared_ptr<RunLoop> &runLoop,
                           const shared_ptr<ThreadPool> &threadPool)
        : runLoop_{runLoop},
          handler_{handler},
          handlerActivationId_{handlerActivationId},
          finished_{false} {
    runLoopClientId_   = runLoop_->AddClient(this);
    synchronizedState_ = make_unique<SynchronizedState>(runLoop_, runLoopClientId_);
    connector_         = make_unique<IO::TcpConnector>(hostAndPort, synchronizedState_.get(), 0, threadPool);
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
                handler_->OnTcpConnectionEstablished(handlerActivationId_, *fd);
            } else {
                handler_->OnFailedToEstablishTcpConnection(handlerActivationId_);
            }
        }
    }
}

}    // Namespace Framework.
}    // Namespace IO.
}    // Namespace K.
