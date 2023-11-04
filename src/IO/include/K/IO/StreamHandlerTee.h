/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's C++ Crossplatform Assets                                                                        ///
//    (C) Copyright Kai Hergenröther. All rights reserved.                                                 //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_IO_STREAMHANDLERTEE_H_
#define K_IO_STREAMHANDLERTEE_H_

#include <memory>
#include <K/Core/StreamHandlerInterface.h>

namespace K {
namespace IO {

//! Stream handler tee piece: forwards a stream to two stream handlers.
class StreamHandlerTee : public virtual Core::StreamHandlerInterface {
  public:
    StreamHandlerTee(const std::shared_ptr<StreamHandlerInterface> &streamHandler1, int activationId1,
                     const std::shared_ptr<StreamHandlerInterface> &streamHandler2, int activationId2);
    StreamHandlerTee(const StreamHandlerTee &other)            = delete;
    StreamHandlerTee &operator=(const StreamHandlerTee &other) = delete;
    StreamHandlerTee(StreamHandlerTee &&other)                 = delete;
    StreamHandlerTee &operator=(StreamHandlerTee &&other)      = delete;

    void OnStreamData(int id, const void *data, int dataSize) override;
    void OnStreamEnteredErrorState(int id, Core::StreamInterface::Error error) override;

  private:
    std::shared_ptr<Core::StreamHandlerInterface> streamHandler1_;
    int                                           activationId1_;
    std::shared_ptr<Core::StreamHandlerInterface> streamHandler2_;
    int                                           activationId2_;
};

}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_STREAMHANDLERTEE_H_

