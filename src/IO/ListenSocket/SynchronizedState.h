/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_IO_LISTENSOCKET_SYNCHRONIZEDSTATE_H_
#define K_IO_LISTENSOCKET_SYNCHRONIZEDSTATE_H_

#include <deque>
#include <mutex>
#include <optional>
#include <K/IO/ListenSocket.h>
#include <K/IO/Deprecated/ListenSocket.h>

namespace K {
    namespace IO {
        namespace Deprecated {
            class TcpConnection;
        }
    }
}

namespace K {
namespace IO {

//! State shared between threads.
/*!
 *  The class is thread safe (all public instance methods).
 */
class ListenSocket::SynchronizedState : public virtual Deprecated::ListenSocket::HandlerInterface {
  public:
    SynchronizedState(const std::shared_ptr<Core::RunLoop> &runLoop);
    SynchronizedState()                                          = delete;
    SynchronizedState(const SynchronizedState &other)            = delete;
    SynchronizedState &operator=(const SynchronizedState &other) = delete;
    SynchronizedState(SynchronizedState &&other)                 = delete;
    SynchronizedState &operator=(SynchronizedState &&other)      = delete;
    ~SynchronizedState();

    void SetRunLoopClientId(int id);
    void Sync(LoopThreadState *loopThreadState);
    void OnListenSocketAcceptedConnection(const std::shared_ptr<Deprecated::TcpConnection> &connection) override;
    void OnListenSocketAcceptedConnection(int fd) override;
    void OnListenSocketErrorState() override;

  private:
    // Expects lock to be held.
    void RequestSync(std::unique_lock<std::mutex> &critical);
    // Expects lock to be held.
    void DropAcceptedConnections(std::unique_lock<std::mutex> &critical);

    std::mutex lock_;    // Protects everything below...
    const std::shared_ptr<Core::RunLoop> runLoop_;
    std::optional<int>                   runLoopClientId_;
    std::deque<int>                      acceptedConnections_;
    bool                                 error_;
    bool                                 syncRequested_;
};

}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_LISTENSOCKET_SYNCHRONIZEDSTATE_H_
