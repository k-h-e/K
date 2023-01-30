#ifndef K_IO_STREAMSAVER_H_
#define K_IO_STREAMSAVER_H_

#include <memory>
#include <string>
#include <K/Core/BlockingOutStreamInterface.h>
#include <K/Core/StreamHandlerInterface.h>
#include <K/Core/StreamInterface.h>

namespace K {
    namespace IO {
        class Path;
    }
}

namespace K {
namespace IO {

//! Stream handler saving the stream to a file.
class StreamSaver : public virtual Core::StreamHandlerInterface {
  public:
    StreamSaver(const K::IO::Path &fileName);
    StreamSaver()                                    = delete;
    StreamSaver(const StreamSaver &other)            = delete;
    StreamSaver &operator=(const StreamSaver &other) = delete;
    StreamSaver(StreamSaver &&other)                 = delete;
    StreamSaver &operator=(StreamSaver &&other)      = delete;
    ~StreamSaver()                                   = default;

    void OnStreamData(int id, const void *data, int dataSize) override;
    void OnStreamEnteredErrorState(int id, Core::StreamInterface::Error error) override;

  private:
    std::unique_ptr<Core::BlockingOutStreamInterface> fileStream_;
};

}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_STREAMSAVER_H_
