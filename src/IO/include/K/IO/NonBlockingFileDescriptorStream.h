#ifndef K_IO_NONBLOCKINGFILEDESCRIPTORSTREAM_H_
#define K_IO_NONBLOCKINGFILEDESCRIPTORSTREAM_H_

#include <memory>
#include <mutex>
#include <K/IO/NonBlockingStreamInterface.h>

namespace K {
namespace IO {

//! Nonblocking stream working on a <c>UNIX</c> file descriptor.
class NonBlockingFileDescriptorStream : public virtual NonBlockingStreamInterface {
  public:
    //! The socket stream takes ownership over the UNIX file descriptor.
    NonBlockingFileDescriptorStream(int fd);
    NonBlockingFileDescriptorStream(const NonBlockingFileDescriptorStream &other)             = delete;
    NonBlockingFileDescriptorStream &operator=(const NonBlockingFileDescriptorStream &other)  = delete;
    NonBlockingFileDescriptorStream(const NonBlockingFileDescriptorStream &&other)            = delete;
    NonBlockingFileDescriptorStream &operator=(const NonBlockingFileDescriptorStream &&other) = delete;
    ~NonBlockingFileDescriptorStream();

    int Read(void *outBuffer, int bufferSize) override;
    int Write(const void *data, int dataSize) override;
    bool EndOfStream() override;
    bool Error() override;
    void RegisterListener(Listener *listener) override;

  private:
    std::mutex lock_;
    int        fd_;
    bool       eof_;
    bool       error_;
};

}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_NONBLOCKINGFILEDESCRIPTORSTREAM_H_
