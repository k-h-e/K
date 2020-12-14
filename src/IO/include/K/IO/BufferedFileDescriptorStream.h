#ifndef K_IO_BUFFEREDFILEDESCRIPTORSTREAM_H_
#define K_IO_BUFFEREDFILEDESCRIPTORSTREAM_H_

#include <memory>
#include <K/IO/NonBlockingStreamInterface.h>

namespace K {
namespace IO {

class IO;

//! Nonblocking buffered stream working on a <c>UNIX</c> file descriptor.
class BufferedFileDescriptorStream : public virtual NonBlockingStreamInterface {
  public:
    //! The connection takes ownership over the UNIX file descriptor.
    BufferedFileDescriptorStream(int fd, int bufferSizeThreshold, const std::shared_ptr<K::IO::IO> &io);
    BufferedFileDescriptorStream(const BufferedFileDescriptorStream &other)             = delete;
    BufferedFileDescriptorStream &operator=(const BufferedFileDescriptorStream &other)  = delete;
    BufferedFileDescriptorStream(const BufferedFileDescriptorStream &&other)            = delete;
    BufferedFileDescriptorStream &operator=(const BufferedFileDescriptorStream &&other) = delete;

    void Register(HandlerInterface *handler) override;
    int Read(void *outBuffer, int bufferSize) override;
    int Write(const void *data, int dataSize) override;
    bool Eof() override;
    bool Error() override;

  private:
    class SharedState;

    std::shared_ptr<SharedState> sharedState_;
};

}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_BUFFEREDFILEDESCRIPTORSTREAM_H_
