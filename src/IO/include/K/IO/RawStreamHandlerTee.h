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

#include <K/Core/BufferQueue.h>
#include <K/Core/RawStreamHandlerInterface.h>

namespace K {
    namespace Core {
        class BufferProviderInterface;
    }
}

namespace K {
namespace IO {

//! Raw stream handler tee piece: forwards a raw stream to two raw stream handlers.
class RawStreamHandlerTee : public virtual Core::RawStreamHandlerInterface {
  public:
    RawStreamHandlerTee(const std::shared_ptr<RawStreamHandlerInterface> &streamHandler1,
                        const std::shared_ptr<RawStreamHandlerInterface> &streamHandler2,
                        const std::shared_ptr<Core::BufferProviderInterface> &bufferProvider);
    RawStreamHandlerTee(const RawStreamHandlerTee &other)            = delete;
    RawStreamHandlerTee &operator=(const RawStreamHandlerTee &other) = delete;
    RawStreamHandlerTee(RawStreamHandlerTee &&other)                 = delete;
    RawStreamHandlerTee &operator=(RawStreamHandlerTee &&other)      = delete;

    void OnRawStreamData(Core::UniqueHandle<Core::ReadableByteSpanInterface> buffer) override;
    void OnStreamError(Core::StreamInterface::Error error) override;

  private:
    std::shared_ptr<Core::RawStreamHandlerInterface> streamHandler1_;
    std::shared_ptr<Core::RawStreamHandlerInterface> streamHandler2_;
    Core::BufferQueue                                workingQueue_;
    std::shared_ptr<Core::BufferProviderInterface>   bufferProvider_;
};

}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_RAWSTREAMHANDLERTEE_H_

