#ifndef K_IO_STREAMHANDLERTEE_H_
#define K_IO_STREAMHANDLERTEE_H_

#include <memory>
#include <K/IO/StreamHandlerInterface.h>

namespace K {
namespace IO {

//! Stream handler tee piece: forwards a stream to two stream handlers.
class StreamHandlerTee : public virtual StreamHandlerInterface {
  public:
    StreamHandlerTee(const std::shared_ptr<StreamHandlerInterface> &streamHandler1,
                     const std::shared_ptr<StreamHandlerInterface> &streamHandler2);
    StreamHandlerTee(const StreamHandlerTee &other)            = delete;
    StreamHandlerTee &operator=(const StreamHandlerTee &other) = delete;
    StreamHandlerTee(StreamHandlerTee &&other)                 = delete;
    StreamHandlerTee &operator=(StreamHandlerTee &&other)      = delete;

    void HandleStreamData(const void *data, int dataSize) override;
    void HandleEof() override;
    void HandleError() override;

  private:
    std::shared_ptr<StreamHandlerInterface> streamHandler1_;
    std::shared_ptr<StreamHandlerInterface> streamHandler2_;
};

}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_STREAMHANDLERTEE_H_

