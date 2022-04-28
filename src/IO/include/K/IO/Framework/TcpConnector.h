////    ////
////   ////     K Crossplatform C++ Assets
////  ////      (C) Copyright Kai Hergenröther
//// ////
////////        - IO / Framework -
//// ////
////  ////
////   ////
////    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef K_IO_FRAMEWORK_TCPCONNECTOR_H_
#define K_IO_FRAMEWORK_TCPCONNECTOR_H_

#include <string>
#include <K/Core/Interface.h>
#include <K/Core/Framework/RunLoop.h>
#include <K/IO/TcpConnector.h>

namespace K {
    namespace Core {
        class ThreadPool;
        namespace Framework {
            class RunLoop;
        }
    }
}

namespace K {
namespace IO {
namespace Framework {

//! Asynchronously establishes a TCP connection to a given host (framework-enabled analogon to IO::TcpConnector).
class TcpConnector : public virtual Core::Interface,
                     private virtual Core::Framework::RunLoop::ClientInterface {
  public:
    //! Establishes a TCP network connection to the specified host, given by name and port, separated by a <c>':'</c>.
    /*!
     *  The handler's methods get activated on the associated run loop's thread. They may call back into the
     *  TcpConnector.
     *
     *  The handler is expected to outlive the TcpConnector. It will not get called upon TcpConnector destruction.
     *
     *  \param handlerActivationId
     *  ID to be passed along with handler activations from this TcpConnector. Useful in case one wants to use one
     *  handler with multiple TcpConnector s.
     */
    TcpConnector(const std::string &hostAndPort, IO::TcpConnector::HandlerInterface *handler, int handlerActivationId,
                 const std::shared_ptr<Core::Framework::RunLoop> &runLoop,
                 const std::shared_ptr<Core::ThreadPool> &threadPool);
    TcpConnector()                                      = delete;
    TcpConnector(const TcpConnector &other)             = delete;
    TcpConnector &operator=(const TcpConnector &other)  = delete;
    TcpConnector(const TcpConnector &&other)            = delete;
    TcpConnector &operator=(const TcpConnector &&other) = delete;
    ~TcpConnector();

  private:
    class SynchronizedState;

    void Activate(bool deepActivation) override;

    const std::shared_ptr<Core::Framework::RunLoop> runLoop_;
    int                                             runLoopClientId_;
    IO::TcpConnector::HandlerInterface              *handler_;
    int                                             handlerActivationId_;
    std::unique_ptr<SynchronizedState>              synchronizedState_;
    std::unique_ptr<IO::TcpConnector>               connector_;
    bool                                            finished_;
};

}    // Namespace Framework.
}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_FRAMEWORK_TCPCONNECTOR_H_
