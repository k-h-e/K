////    ////
////   ////     K Crossplatform C++ Assets
////  ////      (C) Copyright Kai Hergenröther
//// ////
////////        - IO -
//// ////
////  ////
////   ////
////    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Connector.h"

#include <K/IO/NetworkTools.h>

using std::optional;
using std::string;
using K::IO::NetworkTools;

namespace K {
namespace IO {

TcpConnector::Connector::Connector(const std::string &hostAndPort, HandlerInterface *handler,
                                   int handlerActivationId)
        : hostAndPort_{hostAndPort},
          handler_{handler},
          handlerActivationId_{handlerActivationId} {
    // Nop.
}

void TcpConnector::Connector::ExecuteAction() {
    optional<int> fd{NetworkTools::ConnectTcp(hostAndPort_, this)};
    if (fd) {
        handler_->OnTcpConnectionEstablished(handlerActivationId_, *fd);
    } else {
        handler_->OnFailedToEstablishTcpConnection(handlerActivationId_);
    }
}

}    // Namespace IO.
}    // Namespace K.
