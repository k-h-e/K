#ifndef K_IO_BUFFEREDCONNECTION_H_
#define K_IO_BUFFEREDCONNECTION_H_

#include <memory>
#include <K/IO/ConnectionStreamInterface.h>

namespace K {
namespace IO {

class ConnectionIO;

//! Buffered connection.
class BufferedConnection : public virtual ConnectionStreamInterface {
  public:
    //! The connection takes ownership over the UNIX file descriptor.
    /*!
     *  Bad file descriptors < 0 are accepted, with the connection being initialized to error state right away.
     */
    BufferedConnection(int fd, int bufferSizeThreshold, const std::shared_ptr<Core::Result> &resultAcceptor,
                       const std::shared_ptr<K::IO::ConnectionIO> &connectionIO);
    BufferedConnection(const BufferedConnection &other)             = delete;
    BufferedConnection &operator=(const BufferedConnection &other)  = delete;
    BufferedConnection(const BufferedConnection &&other)            = delete;
    BufferedConnection &operator=(const BufferedConnection &&other) = delete;
    ~BufferedConnection();

    bool Register(const std::shared_ptr<HandlerInterface> &handler) override;
    void Unregister(const std::shared_ptr<HandlerInterface> &handler) override;
    bool WriteItem(const void *item, int itemSize) override;
    bool Eof() override;
    bool ErrorState() override;
    void SetFinalResultAcceptor(const std::shared_ptr<Core::Result> &resultAcceptor) override;

  private:
    class SharedState;

    std::shared_ptr<SharedState> sharedState_;

    std::shared_ptr<K::IO::ConnectionIO> connectionIO_;
    int                                  fd_;
    std::shared_ptr<Core::Result>        finalResultAcceptor_;
};

}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_BUFFEREDCONNECTION_H_
