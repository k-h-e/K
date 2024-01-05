/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_IO_TCPCONNECTOR_H_
#define K_IO_TCPCONNECTOR_H_

#include <string>
#include <K/Core/Interface.h>
#include <K/Core/RunLoop.h>
#include <K/IO/Deprecated/TcpConnector.h>

namespace K {
    namespace Core {
        class ThreadPool;
    }
}

namespace K {
namespace IO {

//! Asynchronously establishes a TCP connection to a given host (framework-enabled analogon to IO::TcpConnector).
class TcpConnector : public virtual Core::Interface,
                     private virtual Core::RunLoop::ClientInterface {
  public:
    //! Establishes a TCP network connection to the specified host, given by name and port, separated by a <c>':'</c>.
    /*!
     *  The handler's methods get activated on the associated run loop's thread. They may call back into the
     *  TcpConnector.
     *
     *  The handler is expected to outlive the TcpConnector. It will not get called upon TcpConnector destruction.
     */
    TcpConnector(const std::string &hostAndPort, IO::Deprecated::TcpConnector::HandlerInterface *handler,
                 const std::shared_ptr<Core::RunLoop> &runLoop, const std::shared_ptr<Core::ThreadPool> &threadPool);
    TcpConnector()                                      = delete;
    TcpConnector(const TcpConnector &other)             = delete;
    TcpConnector &operator=(const TcpConnector &other)  = delete;
    TcpConnector(const TcpConnector &&other)            = delete;
    TcpConnector &operator=(const TcpConnector &&other) = delete;
    ~TcpConnector();

  private:
    class SynchronizedState;

    void Activate(bool deepActivation) override;

    const std::shared_ptr<Core::RunLoop>           runLoop_;
    int                                            runLoopClientId_;
    IO::Deprecated::TcpConnector::HandlerInterface *handler_;
    std::unique_ptr<SynchronizedState>             synchronizedState_;
    std::unique_ptr<IO::Deprecated::TcpConnector>  connector_;
    bool                                           finished_;
};

}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_TCPCONNECTOR_H_
