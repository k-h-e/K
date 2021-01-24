#ifndef K_IO_STREAMBUFFER_H_
#define K_IO_STREAMBUFFER_H_

#include <memory>
#include <K/IO/BlockingStreamCore.h>
#include <K/IO/SeekableBlockingStreamInterface.h>

namespace K {
namespace IO {

//! Decorator adding I/O buffering to a seekable blocking stream.
class StreamBuffer : public BlockingStreamCore,
                     public virtual SeekableBlockingStreamInterface {
  public:
    StreamBuffer(const std::shared_ptr<SeekableBlockingStreamInterface> &stream, int bufferSize);
    StreamBuffer(const StreamBuffer &other)             = delete;
    StreamBuffer &operator=(const StreamBuffer &other)  = delete;
    StreamBuffer(const StreamBuffer &&other)            = delete;
    StreamBuffer &operator=(const StreamBuffer &&other) = delete;
    ~StreamBuffer();

    int Read(void *outBuffer, int bufferSize) override;
    int Write(const void *data, int dataSize) override;
    bool Seek(int64_t position) override;
    int64_t StreamPosition() override;
    bool Eof() override;
    bool ErrorState() override;

  private:
    int64_t BufferPositionFor(int64_t position);
    bool Flush();
    bool SetUpBuffer(int64_t position);

    std::shared_ptr<SeekableBlockingStreamInterface> stream_;
    uint8_t                                          *buffer_;
    int                                              bufferSize_;
    int64_t                                          bufferPosition_;
    int                                              cursor_;
    int                                              fill_;
    bool                                             dirty_;
    bool                                             error_;
};

}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_STREAMBUFFER_H_
