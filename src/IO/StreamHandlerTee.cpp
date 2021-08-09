#include <K/IO/StreamHandlerTee.h>

using std::shared_ptr;
using K::IO::StreamHandlerInterface;

namespace K {
namespace IO {

StreamHandlerTee::StreamHandlerTee(const shared_ptr<StreamHandlerInterface> &streamHandler1,
                                   const shared_ptr<StreamHandlerInterface> &streamHandler2)
        : streamHandler1_(streamHandler1),
          streamHandler2_(streamHandler2) {
    // Nop.
}

void StreamHandlerTee::HandleStreamData(const void *data, int dataSize) {
    streamHandler1_->HandleStreamData(data, dataSize);
    streamHandler2_->HandleStreamData(data, dataSize);
}

void StreamHandlerTee::HandleEof() {
    streamHandler1_->HandleEof();
    streamHandler2_->HandleEof();
}

void StreamHandlerTee::HandleError() {
    streamHandler1_->HandleError();
    streamHandler2_->HandleError();
}

}    // Namespace IO.
}    // Namespace K.
