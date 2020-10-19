#ifndef K_IO_SOCKETSTREAM_H_
#define K_IO_SOCKETSTREAM_H_

#include <memory>
#include <mutex>
#include <K/IO/BlockingStreamCore.h>

namespace K {
namespace IO {

//! Socket stream.
/*!
 *  The class is thread-safe (i.e. all public methods).
 */
class SocketStream : public BlockingStreamCore {
  public:
    //! The socket stream takes ownership over the UNIX file descriptor.
    SocketStream(int fd);
    SocketStream(const SocketStream &other)             = delete;
    SocketStream &operator=(const SocketStream &other)  = delete;
    SocketStream(const SocketStream &&other)            = delete;
    SocketStream &operator=(const SocketStream &&other) = delete;
    ~SocketStream();

    //! Shuts down the underlying socket if it is still up.
    void ShutDown();

    int Read(void *outBuffer, int bufferSize) override;
    int Write(const void *data, int dataSize) override;
    bool EndOfStream() override;
    bool Error() override;

    //! Establishes a network stream connection to a host given by name and port, separated by a <c>':'</c>.
    /*!
     *  \return
     *  <c>null</c> handle in case of failure.
     */
    static std::shared_ptr<SocketStream> ConnectToHost(const std::string &host);
    //! Establishes a network stream connection to the specified host.
    /*!
     *  \return
     *  <c>null</c> handle in case of failure.
     */
    static std::shared_ptr<SocketStream> ConnectToHost(uint32_t ip4Address, int port);
    //! Resolves the specified host name.
    /*!
     *  \return
     *  <c>false</c> in case of failure. The output parameter will then be undefined.
     */
    static bool ResolveHostName(const std::string &hostName, uint32_t *outIp4Address);
    //! Returns a string representation for the specified IP4 address.
    static std::string Ip4ToString(uint32_t ip4Address);

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

#endif    // K_IO_SOCKETSTREAM_H_
