/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's C++ Crossplatform Assets                                                                        ///
//    (C) Copyright Kai Hergenröther. All rights reserved.                                                 //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_IO_STREAMBUFFER_H_
#define K_IO_STREAMBUFFER_H_

#include <memory>
#include <vector>
#include <K/Core/SeekableBlockingIOStreamInterface.h>
#include <K/IO/File.h>

namespace K {
namespace IO {

//! Decorator adding I/O buffering to a seekable blocking stream.
class StreamBuffer : public virtual Core::SeekableBlockingIOStreamInterface {
  public:
    StreamBuffer(const std::shared_ptr<Core::SeekableBlockingIOStreamInterface> &stream, File::AccessMode accessMode,
                 int bufferSize);
    StreamBuffer(const StreamBuffer &other)             = delete;
    StreamBuffer &operator=(const StreamBuffer &other)  = delete;
    StreamBuffer(const StreamBuffer &&other)            = delete;
    StreamBuffer &operator=(const StreamBuffer &&other) = delete;
    ~StreamBuffer();

    int ReadBlocking(void *buffer, int bufferSize) override;
    int WriteBlocking(const void *data, int dataSize) override;
    void Seek(int64_t position) override;
    void RecoverAndSeek(int64_t position) override;
    int64_t StreamPosition() const override;
    bool ErrorState() const override;
    std::optional<Error> StreamError() const override;
    void SetCloseResultAcceptor(const std::shared_ptr<Core::ResultAcceptor> &resultAcceptor) override;

  private:
    int64_t BufferPositionFor(int64_t position);
    bool SetUpBuffer(int64_t position);
    bool Flush();
    bool FlushDirtyRange(int cursor, int numBytes);

    std::shared_ptr<Core::SeekableBlockingIOStreamInterface> stream_;
    uint8_t                                                  *buffer_;
    int                                                      bufferSize_;
    bool                                                     readable_;
    bool                                                     writable_;
    int64_t                                                  bufferPosition_;
    int                                                      cursor_;
    int                                                      fill_;
    std::vector<bool>                                        dirtyBytes_;             // Only used in write-only mode.
    bool                                                     dirty_;
    std::optional<Error>                                     error_;
    std::shared_ptr<Core::ResultAcceptor>                    closeResultAcceptor_;
};

}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_STREAMBUFFER_H_
