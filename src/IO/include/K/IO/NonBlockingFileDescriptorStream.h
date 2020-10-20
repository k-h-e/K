#ifndef K_IO_NONBLOCKINGFILEDESCRIPTORSTREAM_H_
#define K_IO_NONBLOCKINGFILEDESCRIPTORSTREAM_H_

#include <memory>
#include <mutex>
#include <K/IO/NonBlockingStreamInterface.h>
#include <K/IO/IO.h>

namespace K {
namespace IO {

//! Nonblocking stream working on a <c>UNIX</c> file descriptor.
class NonBlockingFileDescriptorStream : public virtual NonBlockingStreamInterface,
                                        public virtual IO::ReadHandlerInterface,
                                        public virtual IO::WriteHandlerInterface {
  public:
    //! The socket stream takes ownership over the UNIX file descriptor.
    NonBlockingFileDescriptorStream(int fd, const std::shared_ptr<IO> &io);
    NonBlockingFileDescriptorStream(const NonBlockingFileDescriptorStream &other)             = delete;
    NonBlockingFileDescriptorStream &operator=(const NonBlockingFileDescriptorStream &other)  = delete;
    NonBlockingFileDescriptorStream(const NonBlockingFileDescriptorStream &&other)            = delete;
    NonBlockingFileDescriptorStream &operator=(const NonBlockingFileDescriptorStream &&other) = delete;
    ~NonBlockingFileDescriptorStream();

    int Read(void *outBuffer, int bufferSize) override;
    int Write(const void *data, int dataSize) override;
    bool EndOfStream() override;
    bool Error() override;
    void Register(HandlerInterface *handler) override;
    void OnReadyRead() override;
    void OnReadyWrite() override;

  private:
    std::shared_ptr<IO> io_;

    std::mutex          lock_;
    int                 fd_;
    bool                eof_;
    bool                error_;
};

}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_NONBLOCKINGFILEDESCRIPTORSTREAM_H_
