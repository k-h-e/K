#ifndef K_IO_NONBLOCKINGFILEDESCRIPTORSTREAM_H_
#define K_IO_NONBLOCKINGFILEDESCRIPTORSTREAM_H_

#include <memory>
#include <mutex>
#include <K/IO/BufferedConnectionStreamInterface.h>
#include <K/IO/IO.h>
#include <K/IO/RingBuffer.h>

namespace K {
namespace IO {

//! Nonblocking stream working on a <c>UNIX</c> file descriptor.
class NonBlockingFileDescriptorStream : public virtual BufferedConnectionStreamInterface,
                                        public virtual IO::ClientInterface {
  public:
    //! The socket stream takes ownership over the UNIX file descriptor.
    NonBlockingFileDescriptorStream(int fd, int bufferSizeThreshold, const std::shared_ptr<IO> &io);
    NonBlockingFileDescriptorStream(const NonBlockingFileDescriptorStream &other)             = delete;
    NonBlockingFileDescriptorStream &operator=(const NonBlockingFileDescriptorStream &other)  = delete;
    NonBlockingFileDescriptorStream(const NonBlockingFileDescriptorStream &&other)            = delete;
    NonBlockingFileDescriptorStream &operator=(const NonBlockingFileDescriptorStream &&other) = delete;
    ~NonBlockingFileDescriptorStream();

    void Register(HandlerInterface *handler) override;
    void Register(ReadHandlerInterface *handler) override;
    int Read(void *outBuffer, int bufferSize) override;
    int Write(const void *data, int dataSize) override;
    bool WriteItem(const void *item, int itemSize) override;
    bool EndOfStream() override;
    bool Error() override;

    bool OnDataRead(void *data, int dataSize) override;
    void OnReadyWrite() override;
    void OnEof() override;
    void OnError() override;

  private:
    std::shared_ptr<IO> io_;

    std::mutex          lock_;
    int                 fd_;
    HandlerInterface    *handler_;
    RingBuffer          readBuffer_;
    RingBuffer          writeBuffer_;
    int                 bufferSizeThreshold_;
    bool                needToSignalReadyRead_;
    bool                canNotRead_;
    bool                canNotWrite_;
    bool                eof_;
    bool                error_;
};

}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_NONBLOCKINGFILEDESCRIPTORSTREAM_H_
