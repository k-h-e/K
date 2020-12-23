#ifndef K_IO_BUFFEREDFILEDESCRIPTORCONNECTION_SHAREDSTATE_H_
#define K_IO_BUFFEREDFILEDESCRIPTORCONNECTION_SHAREDSTATE_H_

#include <memory>
#include <mutex>
#include <condition_variable>
#include <K/Core/RingBuffer.h>
#include <K/IO/BufferedFileDescriptorConnection.h>
#include <K/IO/IO.h>

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
    bool WriteItem(const void *item, int itemSize);
    bool Eof();
    bool Error();

    bool OnDataRead(const void *data, int dataSize) override;
    int OnReadyWrite(void *buffer, int bufferSize) override;
    void OnIncompleteWrite(const void *unwrittenData, int unwrittenDataSize) override;
    void OnEof() override;
    void OnError() override;

  private:
    // Expects lock to be held.
    void EnsureHandlerCalledInitially();

    std::mutex              lock_;    // Protects everything below...

    std::condition_variable writeCanContinue_;
    std::shared_ptr<IO>     io_;
    int                     fd_;
    HandlerInterface        *handler_;
    bool                    handlerCalledInitially_;
    Core::RingBuffer        writeBuffer_;
    int                     bufferSizeThreshold_;
    bool                    canNotWrite_;
    bool                    eof_;
    bool                    error_;
};

}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_BUFFEREDFILEDESCRIPTORCONNECTION_SHAREDSTATE_H_
