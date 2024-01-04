/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther.                                                                      //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_IO_RAWSTREAMSAVER_H_
#define K_IO_RAWSTREAMSAVER_H_

#include <memory>
#include <string>
#include <K/Core/BlockingOutStreamInterface.h>
#include <K/Core/RawStreamHandlerInterface.h>
#include <K/Core/StreamInterface.h>

namespace K {
    namespace IO {
        class Path;
    }
}

namespace K {
namespace IO {

//! Raw stream handler saving the raw stream to a file.
class RawStreamSaver : public virtual Core::RawStreamHandlerInterface {
  public:
    RawStreamSaver(const K::IO::Path &fileName);
    RawStreamSaver()                                       = delete;
    RawStreamSaver(const RawStreamSaver &other)            = delete;
    RawStreamSaver &operator=(const RawStreamSaver &other) = delete;
    RawStreamSaver(RawStreamSaver &&other)                 = delete;
    RawStreamSaver &operator=(RawStreamSaver &&other)      = delete;
    ~RawStreamSaver()                                      = default;

    void OnRawStreamData(const void *data, int dataSize) override;
    void OnStreamError(Core::StreamInterface::Error error) override;

  private:
    std::unique_ptr<Core::BlockingOutStreamInterface> fileStream_;
};

}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_RAWSTREAMSAVER_H_
