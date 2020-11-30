#ifndef K_IO_BUFFEREDFILEDESCRIPTORCONNECTION_H_
#define K_IO_BUFFEREDFILEDESCRIPTORCONNECTION_H_

#include <memory>
#include <K/IO/BufferedConnectionStreamInterface.h>

namespace K {
namespace IO {

class IO;

//! Buffered connection working on a <c>UNIX</c> file descriptor.
class BufferedFileDescriptorConnection : public virtual BufferedConnectionStreamInterface {
  public:
    //! The connection takes ownership over the UNIX file descriptor.
    BufferedFileDescriptorConnection(int fd, int bufferSizeThreshold, const std::shared_ptr<K::IO::IO> &io);
    BufferedFileDescriptorConnection(const BufferedFileDescriptorConnection &other)             = delete;
    BufferedFileDescriptorConnection &operator=(const BufferedFileDescriptorConnection &other)  = delete;
    BufferedFileDescriptorConnection(const BufferedFileDescriptorConnection &&other)            = delete;
    BufferedFileDescriptorConnection &operator=(const BufferedFileDescriptorConnection &&other) = delete;

    void Register(HandlerInterface *handler) override;
    void Register(ReadHandlerInterface *handler) override;
    int Read(void *outBuffer, int bufferSize) override;
    int Write(const void *data, int dataSize) override;
    bool WriteItem(const void *item, int itemSize) override;
    bool Eof() override;
    bool Error() override;

  private:
    class SharedState;

    std::shared_ptr<SharedState> sharedState_;
};

}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_BUFFEREDFILEDESCRIPTORCONNECTION_H_
