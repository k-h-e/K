/*!
 *  \defgroup IO IO
 *  \brief I/O.
 */

#ifndef K_IO_LISTENSOCKET_H_
#define K_IO_LISTENSOCKET_H_

#include <memory>
#include <mutex>
#include <K/Core/ErrorInterface.h>

namespace K {
namespace IO {

class SocketStream;

//! Listen socket.
/*!
 *  \ingroup IO
 *
 * Thread-safe (i.e. all public methods).
 */
class ListenSocket : public virtual K::Core::ErrorInterface {
  public:
    ListenSocket(int port);
    ListenSocket(const ListenSocket &other)             = delete;
    ListenSocket &operator=(const ListenSocket &other)  = delete;
    ListenSocket(const ListenSocket &&other)            = delete;
    ListenSocket &operator=(const ListenSocket &&other) = delete;
    ~ListenSocket();

    //! Accepts a new connection from the listen socket.
    /*!
     *  \return
     *  <c>null</c> handle in case of failure.
     */
    std::shared_ptr<SocketStream> Accept();
    //! Shuts down the listen socket, causing active <c>Accept()</c> calls to return and fail.
    void ShutDown();

    bool Error();

  private:
    void Close();

    std::mutex lock_;
    int        fd_;
    bool       socketDown_;
};

}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_LISTENSOCKET_H_
