/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther.                                                                      //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#include "Connector.h"

#include <K/IO/NetworkTools.h>

using std::optional;
using std::string;
using K::IO::NetworkTools;

namespace K {
namespace IO {
namespace Deprecated {

TcpConnector::Connector::Connector(const std::string &hostAndPort, HandlerInterface *handler)
        : hostAndPort_{hostAndPort},
          handler_{handler} {
    // Nop.
}

void TcpConnector::Connector::ExecuteAction() {
    optional<int> fd{NetworkTools::ConnectTcp(hostAndPort_, this)};
    if (fd) {
        handler_->OnTcpConnectionEstablished(*fd);
    } else {
        handler_->OnFailedToEstablishTcpConnection();
    }
}

}    // Namespace Deprecated.
}    // Namespace IO.
}    // Namespace K.
