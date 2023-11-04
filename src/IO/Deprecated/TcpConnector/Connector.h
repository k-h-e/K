/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's C++ Crossplatform Assets                                                                        ///
//    (C) Copyright Kai Hergenröther. All rights reserved.                                                 //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_IO_DEPRECATED_TCPCONNECTOR_CONNECTOR_H_
#define K_IO_DEPRECATED_TCPCONNECTOR_CONNECTOR_H_

#include <K/Core/ActionInterface.h>
#include <K/IO/Deprecated/TcpConnector.h>

namespace K {
namespace IO {
namespace Deprecated {

//! Connector (thread) for the TcpConnector.
class TcpConnector::Connector : public virtual Core::ActionInterface {
  public:
    Connector(const std::string &hostAndPort, HandlerInterface *handler, int handlerActivationId);
    Connector()                                   = delete;
    Connector(const Connector &other)             = delete;
    Connector &operator=(const Connector &other)  = delete;
    Connector(const Connector &&other)            = delete;
    Connector &operator=(const Connector &&other) = delete;

    void ExecuteAction() override;

  private:
    std::string      hostAndPort_;
    HandlerInterface *handler_;
    int              handlerActivationId_;
};

}    // Namespace Deprecated.
}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_DEPRECATED_TCPCONNECTOR_CONNECTOR_H_
