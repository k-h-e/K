/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's C++ Crossplatform Assets                                                                        ///
//    (C) Copyright Kai Hergenröther. All rights reserved.                                                 //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#include <K/IO/StreamHandlerTee.h>

using std::shared_ptr;
using K::Core::StreamHandlerInterface;
using K::Core::StreamInterface;

namespace K {
namespace IO {

StreamHandlerTee::StreamHandlerTee(const shared_ptr<StreamHandlerInterface> &streamHandler1, int activationId1,
                                   const shared_ptr<StreamHandlerInterface> &streamHandler2, int activationId2)
        : streamHandler1_{streamHandler1},
          activationId1_{activationId1},
          streamHandler2_(streamHandler2),
          activationId2_{activationId2} {
    // Nop.
}

void StreamHandlerTee::OnStreamData(int id, const void *data, int dataSize) {
    (void)id;
    streamHandler1_->OnStreamData(activationId1_, data, dataSize);
    streamHandler2_->OnStreamData(activationId2_, data, dataSize);
}

void StreamHandlerTee::OnStreamEnteredErrorState(int id, StreamInterface::Error error) {
    (void)id;
    streamHandler1_->OnStreamEnteredErrorState(activationId1_, error);
    streamHandler2_->OnStreamEnteredErrorState(activationId2_, error);
}

}    // Namespace IO.
}    // Namespace K.
