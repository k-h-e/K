#ifndef K_IO_STREAMSAVER_H_
#define K_IO_STREAMSAVER_H_

#include <memory>
#include <string>
#include <K/Core/StreamHandlerInterface.h>
#include <K/IO/StreamBuffer.h>

namespace K {
namespace IO {

//! Stream handler saving the stream to a file.
class StreamSaver : public virtual Core::StreamHandlerInterface {
  public:
    StreamSaver(const std::string &fileName);
    StreamSaver(const StreamSaver &other)            = delete;
    StreamSaver &operator=(const StreamSaver &other) = delete;
    StreamSaver(StreamSaver &&other)                 = delete;
    StreamSaver &operator=(StreamSaver &&other)      = delete;

    void HandleStreamData(const void *data, int dataSize) override;
    void HandleEof() override;
    void HandleError() override;

  private:
    std::unique_ptr<StreamBuffer> stream_;
};

}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_STREAMSAVER_H_
