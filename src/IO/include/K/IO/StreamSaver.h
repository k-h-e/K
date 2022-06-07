#ifndef K_IO_STREAMSAVER_H_
#define K_IO_STREAMSAVER_H_

#include <memory>
#include <string>
#include <K/Core/BinaryWriter.h>
#include <K/Core/StreamHandlerInterface.h>


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

    void HandleStreamData(int id, const void *data, int dataSize) override;
    void HandleEof(int id) override;
    void HandleError(int id) override;

  private:
    std::unique_ptr<Core::BinaryWriter> writer_;
};

}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_STREAMSAVER_H_
