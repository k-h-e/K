/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_IO_DEPRECATED_TCPCONNECTOR_H_
#define K_IO_DEPRECATED_TCPCONNECTOR_H_

#include <memory>
#include <string>
#include <K/Core/Interface.h>

namespace K {
    namespace Core {
        class ThreadPool;
    }
}

namespace K {
namespace IO {
namespace Deprecated {

//! Asynchronously establishes a TCP connection to a given host.
class TcpConnector : public virtual Core::Interface {
  public:
    //! Interface to TcpConnector handlers.
    /*!
     *  Only one of the handler methods will get called, and only once. After that, the TcpConnector is inactive until
     *  destroyed.
     */
    class HandlerInterface : public virtual Core::Interface {
      public:
        //! Used by the TcpConnector in case of success to hand over the established TCP connection.
        /*!
         *  The handler is responsible for eventually closing the connection, otherwise it will leak.
         */
        virtual void OnTcpConnectionEstablished(int fd) = 0;
        //! Used by the TcpConnector to indicate failure.
        virtual void OnFailedToEstablishTcpConnection() = 0;
    };

    //! Establishes a TCP network connection to the specified host, given by name and port, separated by a <c>':'</c>.
    /*!
     *  The handler methods will get called on an arbitrary thread and must not call back into the \p TcpConnector.
     *
     *  The handler is expected to outlive the TcpConnector.
     */
    TcpConnector(const std::string &hostAndPort, HandlerInterface *handler,
                 const std::shared_ptr<Core::ThreadPool> &threadPool);
    TcpConnector()                                      = delete;
    TcpConnector(const TcpConnector &other)             = delete;
    TcpConnector &operator=(const TcpConnector &other)  = delete;
    TcpConnector(const TcpConnector &&other)            = delete;
    TcpConnector &operator=(const TcpConnector &&other) = delete;
    ~TcpConnector();

  private:
    class Connector;
    class SharedState;

    const std::shared_ptr<SharedState> sharedState_;
    const std::unique_ptr<Connector>   connector_;
};

}    // Namespace Deprecated.
}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_DEPRECATED_TCPCONNECTOR_H_
