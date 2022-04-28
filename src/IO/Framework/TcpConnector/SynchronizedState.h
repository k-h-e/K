////    ////
////   ////     K Crossplatform C++ Assets
////  ////      (C) Copyright Kai Hergenröther
//// ////
////////        - IO / Framework -
//// ////
////  ////
////   ////
////    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef K_IO_FRAMEWORK_TCPCONNECTOR_SYNCHRONIZEDSTATE_H_
#define K_IO_FRAMEWORK_TCPCONNECTOR_SYNCHRONIZEDSTATE_H_

#include <mutex>
#include <K/IO/Framework/TcpConnector.h>

namespace K {
namespace IO {
namespace Framework {

//! Connector state shared between threads.
/*!
 *  The class is thread safe (i.e. all public instance methods are).
 */
class TcpConnector::SynchronizedState : public virtual IO::TcpConnector::HandlerInterface {
  public:
    SynchronizedState(const std::shared_ptr<Core::Framework::RunLoop> &runLoop, int runLoopClientId);
    SynchronizedState()                                          = delete;
    SynchronizedState(const SynchronizedState &other)            = delete;
    SynchronizedState &operator=(const SynchronizedState &other) = delete;
    SynchronizedState(SynchronizedState &&other)                 = delete;
    SynchronizedState &operator=(SynchronizedState &&other)      = delete;
    ~SynchronizedState();

    void Sync(std::optional<int> *fd, bool *finished);

    void OnTcpConnectionEstablished(int id, int fd) override;
    void OnFailedToEstablishTcpConnection(int id) override;

  private:
    std::mutex lock_;    // Protects everything below...
    const std::shared_ptr<Core::Framework::RunLoop> runLoop_;
    int                                             runLoopClientId_;
    std::optional<int>                              fd_;
    bool                                            finished_;
};

}    // Namespace Framework.
}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_FRAMEWORK_TCPCONNECTOR_SYNCHRONIZEDSTATE_H_
