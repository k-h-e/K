#ifndef K_IO_BUFFEREDFILEDESCRIPTORCONNECTION_SHAREDSTATE_H_
#define K_IO_BUFFEREDFILEDESCRIPTORCONNECTION_SHAREDSTATE_H_

#include <memory>
#include <mutex>
#include <K/IO/BufferedFileDescriptorConnection.h>
#include <K/IO/IO.h>
#include <K/IO/RingBuffer.h>

namespace K {
namespace IO {

//! State shared between threads of the buffered connection.
class BufferedFileDescriptorConnection::SharedState : public virtual IO::ClientInterface {
  public:
    SharedState(int fd, int bufferSizeThreshold, const std::shared_ptr<K::IO::IO> &io);
    SharedState(const SharedState &other)             = delete;
    SharedState &operator=(const SharedState &other)  = delete;
    SharedState(const SharedState &&other)            = delete;
    SharedState &operator=(const SharedState &&other) = delete;
    ~SharedState();

    void Register(HandlerInterface *handler);
    void Register(ReadHandlerInterface *handler);
    int Read(void *outBuffer, int bufferSize);
    int Write(const void *data, int dataSize);
    bool WriteItem(const void *item, int itemSize);
    bool Eof();
    bool Error();

    bool OnDataRead(void *data, int dataSize) override;
    void OnReadyWrite() override;
    void OnEof() override;
    void OnError() override;

  private:
    std::mutex          lock_;
    std::shared_ptr<IO> io_;
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

#endif    // K_IO_BUFFEREDFILEDESCRIPTORCONNECTION_SHAREDSTATE_H_
