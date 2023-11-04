/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's C++ Crossplatform Assets                                                                        ///
//    (C) Copyright Kai Hergenröther. All rights reserved.                                                 //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_IO_DEPRECATED_LISTENSOCKET_SHAREDSTATE_H_
#define K_IO_DEPRECATED_LISTENSOCKET_SHAREDSTATE_H_

#include <condition_variable>
#include <memory>
#include <mutex>
#include <K/Core/CompletionHandlerInterface.h>
#include <K/Core/ErrorStateInterface.h>
#include <K/IO/Deprecated/ListenSocket.h>

namespace K {
    namespace Core {
        class ThreadPool;
    }
    namespace IO {
        class Socket;
        class ConnectionIO;
        namespace Deprecated {
            class TcpConnection;
        }
    }
}

namespace K {
namespace IO {
namespace Deprecated {

//! Shared state for the listen socket.
/*!
 * Thread-safe (i.e. all public methods).
 */
class ListenSocket::SharedState : public virtual K::Core::ErrorStateInterface,
                                  public virtual K::Core::CompletionHandlerInterface {
  public:
    SharedState(int port, const std::shared_ptr<K::IO::ConnectionIO> &connectionIO,
                const std::shared_ptr<K::Core::ThreadPool> &threadPool);
    SharedState(const SharedState &other)             = delete;
    SharedState &operator=(const SharedState &other)  = delete;
    SharedState(SharedState &&other)                  = delete;
    SharedState &operator=(SharedState &&other)       = delete;
    ~SharedState();

    void Register(HandlerInterface *handler, bool fileDescriptorMode);
    bool ErrorState() const override;

    void OnConnectionAccepted(int fd);
    void ReportError();
    void OnCompletion(int completionId) override;

  private:
    // Expects lock to be held.
    void UpdateAcceptor(std::unique_lock<std::mutex> &critical);
    // Expects lock to be held.
    void EnsureHandlerUpdatedInitially(std::unique_lock<std::mutex> &critical);

    mutable std::mutex                   lock_;    // Protects everything below...
    std::condition_variable              stateChanged_;
    int                                  port_;
    HandlerInterface                     *handler_;
    bool                                 handlerFileDescriptorMode_;
    bool                                 handlerUpdatedInitially_;
    std::unique_ptr<Acceptor>            acceptor_;
    bool                                 acceptorThreadRunning_;
    bool                                 error_;
    bool                                 shuttingDown_;
    std::shared_ptr<K::IO::ConnectionIO> connectionIO_;
    std::shared_ptr<K::Core::ThreadPool> threadPool_;
};

}    // Namespace Deprecated.
}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_DEPRECATED_LISTENSOCKET_SHAREDSTATE_H_
