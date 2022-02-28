#ifndef K_IO_STREAMBUFFER_H_
#define K_IO_STREAMBUFFER_H_

#include <memory>
#include <vector>
#include <K/IO/BlockingStreamCore.h>
#include <K/IO/SeekableBlockingStreamInterface.h>
#include <K/IO/File.h>

namespace K {
namespace IO {

//! Decorator adding I/O buffering to a seekable blocking stream.
class StreamBuffer : public BlockingStreamCore,
                     public virtual SeekableBlockingStreamInterface {
  public:
    StreamBuffer(const std::shared_ptr<SeekableBlockingStreamInterface> &stream, File::AccessMode accessMode,
                 int bufferSize);
    StreamBuffer(const std::shared_ptr<SeekableBlockingStreamInterface> &stream, File::AccessMode accessMode,
                 int bufferSize, const std::shared_ptr<Core::Result> &resultAcceptor);
    StreamBuffer(const StreamBuffer &other)             = delete;
    StreamBuffer &operator=(const StreamBuffer &other)  = delete;
    StreamBuffer(const StreamBuffer &&other)            = delete;
    StreamBuffer &operator=(const StreamBuffer &&other) = delete;
    ~StreamBuffer();

    int Read(void *outBuffer, int bufferSize) override;
    int Write(const void *data, int dataSize) override;
    void Seek(int64_t position) override;
    int64_t StreamPosition() override;
    bool Good() const override;
    bool Eof() const override;
    void ClearEof() override;
    bool ErrorState() const override;
    void SetFinalResultAcceptor(const std::shared_ptr<Core::Result> &resultAcceptor) override;

  private:
    int64_t BufferPositionFor(int64_t position);
    bool SetUpBuffer(int64_t position);
    bool Flush();
    bool FlushDirtyRange(int cursor, int numBytes);

    std::shared_ptr<SeekableBlockingStreamInterface> stream_;
    uint8_t                                          *buffer_;
    int                                              bufferSize_;
    bool                                             readable_;
    bool                                             writable_;
    int64_t                                          bufferPosition_;
    int                                              cursor_;
    int                                              fill_;
    std::vector<bool>                                dirtyBytes_;        // Only used in write-only mode.
    bool                                             dirty_;
    bool                                             eof_;
    bool                                             error_;
    std::shared_ptr<Core::Result>                    finalResultAcceptor_;
};

}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_STREAMBUFFER_H_
