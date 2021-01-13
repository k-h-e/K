#ifndef K_IO_SOCKET_H_
#define K_IO_SOCKET_H_

#include <memory>
#include <mutex>
#include <K/IO/BlockingStreamCore.h>

namespace K {
namespace IO {

//! Socket.
/*!
 *  The class is thread-safe (i.e. all public instance methods).
 */
class Socket : public BlockingStreamCore {
  public:
    //! The socket takes ownership over the UNIX file descriptor.
    Socket(int fd);
    Socket(const Socket &other)             = delete;
    Socket &operator=(const Socket &other)  = delete;
    Socket(const Socket &&other)            = delete;
    Socket &operator=(const Socket &&other) = delete;
    ~Socket();

    //! Shuts down the socket if it is still up.
    void ShutDown();

    int Read(void *outBuffer, int bufferSize) override;
    int Write(const void *data, int dataSize) override;
    bool Eof() override;
    bool ErrorState() override;

    //! Establishes a TCP network stream connection to a host given by name and port, separated by a <c>':'</c>.
    /*!
     *  \return
     *  <c>null</c> handle in case of failure.
     */
    static std::shared_ptr<Socket> ConnectTcp(const std::string &host, K::Core::Interface *loggingObject);
    //! Establishes a TCP network stream connection to the specified host.
    /*!
     *  \return
     *  <c>null</c> handle in case of failure.
     */
    static std::shared_ptr<Socket> ConnectTcp(uint32_t ip4Address, int port, K::Core::Interface *loggingObject);

  private:
    void ShutDownSocket();

    std::mutex lock_;
    int        fd_;
    bool       socketDown_;
    bool       eof_;
    bool       error_;
};

}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_SOCKET_H_
