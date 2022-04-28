////    ////
////   ////     K Crossplatform C++ Assets
////  ////      (C) Copyright Kai Hergenröther
//// ////
////////        - IO -
//// ////
////  ////
////   ////
////    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <K/IO/TcpConnector.h>

#include <K/Core/ThreadPool.h>
#include "Connector.h"
#include "SharedState.h"

using std::make_shared;
using std::make_unique;
using std::shared_ptr;
using std::string;
using K::Core::ThreadPool;

namespace K {
namespace IO {

TcpConnector::TcpConnector(const string &hostAndPort, HandlerInterface *handler, int handlerActivationId,
                           const shared_ptr<ThreadPool> &threadPool)
        : sharedState_{make_shared<SharedState>()},
          connector_{make_unique<Connector>(hostAndPort, handler, handlerActivationId)} {
    threadPool->Run(connector_.get(), sharedState_.get(), 0);
}

TcpConnector::~TcpConnector() {
    sharedState_->WaitForConnectorFinished();
}

}    // Namespace IO.
}    // Namespace K.
