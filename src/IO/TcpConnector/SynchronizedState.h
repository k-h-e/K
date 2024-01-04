/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's C++ Crossplatform Assets                                                                        ///
//    (C) Copyright Kai Hergenröther.                                                                      //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_IO_TCPCONNECTOR_SYNCHRONIZEDSTATE_H_
#define K_IO_TCPCONNECTOR_SYNCHRONIZEDSTATE_H_

#include <mutex>
#include <optional>
#include <K/IO/TcpConnector.h>

namespace K {
namespace IO {

//! Connector state shared between threads.
/*!
 *  The class is thread safe (i.e. all public instance methods are).
 */
class TcpConnector::SynchronizedState : public virtual IO::Deprecated::TcpConnector::HandlerInterface {
  public:
    SynchronizedState(const std::shared_ptr<Core::RunLoop> &runLoop, int runLoopClientId);
    SynchronizedState()                                          = delete;
    SynchronizedState(const SynchronizedState &other)            = delete;
    SynchronizedState &operator=(const SynchronizedState &other) = delete;
    SynchronizedState(SynchronizedState &&other)                 = delete;
    SynchronizedState &operator=(SynchronizedState &&other)      = delete;
    ~SynchronizedState();

    void Sync(std::optional<int> *fd, bool *finished);

    void OnTcpConnectionEstablished(int fd) override;
    void OnFailedToEstablishTcpConnection() override;

  private:
    std::mutex lock_;    // Protects everything below...
    const std::shared_ptr<Core::RunLoop> runLoop_;
    int                                  runLoopClientId_;
    std::optional<int>                   fd_;
    bool                                 finished_;
};

}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_TCPCONNECTOR_SYNCHRONIZEDSTATE_H_
