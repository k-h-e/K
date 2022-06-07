#include <K/IO/StreamHandlerTee.h>

using std::shared_ptr;
using K::Core::StreamHandlerInterface;

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

void StreamHandlerTee::HandleStreamData(int id, const void *data, int dataSize) {
    (void)id;
    streamHandler1_->HandleStreamData(activationId1_, data, dataSize);
    streamHandler2_->HandleStreamData(activationId2_, data, dataSize);
}

void StreamHandlerTee::HandleEof(int id) {
    (void)id;
    streamHandler1_->HandleEof(activationId1_);
    streamHandler2_->HandleEof(activationId2_);
}

void StreamHandlerTee::HandleError(int id) {
    (void)id;
    streamHandler1_->HandleError(activationId1_);
    streamHandler2_->HandleError(activationId2_);
}

}    // Namespace IO.
}    // Namespace K.
