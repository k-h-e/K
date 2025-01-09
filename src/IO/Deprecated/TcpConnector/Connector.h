/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
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
    Connector(const std::string &hostAndPort, HandlerInterface *handler);
    Connector()                                   = delete;
    Connector(const Connector &other)             = delete;
    Connector &operator=(const Connector &other)  = delete;
    Connector(const Connector &&other)            = delete;
    Connector &operator=(const Connector &&other) = delete;

    // ActionInterface...
    void Execute() override;

  private:
    std::string      hostAndPort_;
    HandlerInterface *handler_;
};

}    // Namespace Deprecated.
}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_DEPRECATED_TCPCONNECTOR_CONNECTOR_H_
