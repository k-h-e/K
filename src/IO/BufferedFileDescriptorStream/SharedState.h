#ifndef K_IO_BUFFEREDFILEDESCRIPTORCONNECTION_SHAREDSTATE_H_
#define K_IO_BUFFEREDFILEDESCRIPTORCONNECTION_SHAREDSTATE_H_

#include <memory>
#include <mutex>
#include <K/Core/RingBuffer.h>
#include <K/IO/BufferedFileDescriptorStream.h>
#include <K/IO/IO.h>

namespace K {
namespace IO {

//! State shared between threads of the buffered connection.
class BufferedFileDescriptorStream::SharedState : public virtual IO::ClientInterface {
  public:
    SharedState(int fd, int bufferSizeThreshold, const std::shared_ptr<K::IO::IO> &io);
    SharedState(const SharedState &other)             = delete;
    SharedState &operator=(const SharedState &other)  = delete;
    SharedState(const SharedState &&other)            = delete;
    SharedState &operator=(const SharedState &&other) = delete;
    ~SharedState();

    void Register(HandlerInterface *handler);
    int Read(void *outBuffer, int bufferSize);
    int Write(const void *data, int dataSize);
    bool Eof();
    bool Error();

    bool OnDataRead(const void *data, int dataSize) override;
    int OnReadyWrite(void *buffer, int bufferSize) override;
    void OnIncompleteWrite(const void *unwrittenData, int unwrittenDataSize) override;
    void OnEof() override;
    void OnError() override;

  private:
    std::mutex          lock_;
    std::shared_ptr<IO> io_;
    int                 fd_;
    HandlerInterface    *handler_;
    Core::RingBuffer    readBuffer_;
    Core::RingBuffer    writeBuffer_;
    int                 bufferSizeThreshold_;
    bool                needToSignalReadyRead_;
    bool                canNotRead_;
    bool                canNotWrite_;
    bool                eof_;
    bool                error_;
};

}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_BUFFEREDFILEDESCRIPTORCONNECTION_SHAREDSTATE_H_
