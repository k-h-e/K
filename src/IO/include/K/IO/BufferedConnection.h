#ifndef K_IO_BUFFEREDCONNECTION_H_
#define K_IO_BUFFEREDCONNECTION_H_

#include <memory>
#include <K/IO/ConnectionStreamInterface.h>

namespace K {
namespace IO {

class IO;

//! Buffered connection.
class BufferedConnection : public virtual ConnectionStreamInterface {
  public:
    //! The connection takes ownership over the UNIX file descriptor.
    BufferedConnection(int fd, int bufferSizeThreshold, const std::shared_ptr<K::IO::IO> &io);
    BufferedConnection(const BufferedConnection &other)             = delete;
    BufferedConnection &operator=(const BufferedConnection &other)  = delete;
    BufferedConnection(const BufferedConnection &&other)            = delete;
    BufferedConnection &operator=(const BufferedConnection &&other) = delete;
    ~BufferedConnection();

    void Register(HandlerInterface *handler) override;
    bool WriteItem(const void *item, int itemSize) override;
    bool Eof() override;
    bool Error() override;

  private:
    class SharedState;

    std::shared_ptr<SharedState> sharedState_;

    std::shared_ptr<K::IO::IO> io_;
    int                        fd_;
};

}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_BUFFEREDCONNECTION_H_
