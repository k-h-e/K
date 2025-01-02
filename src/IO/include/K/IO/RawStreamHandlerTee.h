/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_IO_RAWSTREAMHANDLERTEE_H_
#define K_IO_RAWSTREAMHANDLERTEE_H_

#include <memory>
#include <K/Core/RawStreamHandlerInterface.h>

namespace K {
    namespace Core {
        class IoBuffers;
    }
}

namespace K {
namespace IO {

//! Raw stream handler tee piece: forwards a raw stream to two raw stream handlers.
class RawStreamHandlerTee : public virtual Core::RawStreamHandlerInterface {
  public:
    RawStreamHandlerTee(const std::shared_ptr<RawStreamHandlerInterface> &streamHandler1,
                        const std::shared_ptr<RawStreamHandlerInterface> &streamHandler2,
                        const std::shared_ptr<Core::IoBuffers> &ioBuffers);
    RawStreamHandlerTee(const RawStreamHandlerTee &other)            = delete;
    RawStreamHandlerTee &operator=(const RawStreamHandlerTee &other) = delete;
    RawStreamHandlerTee(RawStreamHandlerTee &&other)                 = delete;
    RawStreamHandlerTee &operator=(RawStreamHandlerTee &&other)      = delete;

    void OnRawStreamData(Core::UniqueHandle<Core::IoBufferInterface> buffer) override;
    void OnStreamError(Core::StreamInterface::Error error) override;

  private:
    std::shared_ptr<Core::IoBuffers> ioBuffers_;    // Thread-safe.

    std::shared_ptr<Core::RawStreamHandlerInterface> streamHandler1_;
    std::shared_ptr<Core::RawStreamHandlerInterface> streamHandler2_;
};

}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_RAWSTREAMHANDLERTEE_H_

