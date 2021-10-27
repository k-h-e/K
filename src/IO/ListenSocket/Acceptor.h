#ifndef K_IO_LISTENSOCKET_ACCEPTOR_H_
#define K_IO_LISTENSOCKET_ACCEPTOR_H_

#include <K/Core/ActionInterface.h>
#include <K/IO/ListenSocket.h>

namespace K {
namespace IO {

//! Acceptor for the listen socket.
class ListenSocket::Acceptor : public virtual Core::ActionInterface {
  public:
    Acceptor(int fd, SharedState *shared);
    Acceptor(const Acceptor &other)             = delete;
    Acceptor &operator=(const Acceptor &other)  = delete;
    Acceptor(Acceptor &&other)            = delete;
    Acceptor &operator=(Acceptor &&other) = delete;

    void ExecuteAction() override;

  private:
    int         fd_;
    SharedState *shared_;
};

}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_LISTENSOCKET_ACCEPTOR_H_
