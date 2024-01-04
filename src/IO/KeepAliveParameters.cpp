/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's C++ Crossplatform Assets                                                                        ///
//    (C) Copyright Kai Hergenröther.                                                                      //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#include <K/IO/KeepAliveParameters.h>

using std::chrono::milliseconds;

namespace K {
namespace IO {

KeepAliveParameters::KeepAliveParameters(milliseconds sendInterval, milliseconds checkInterval)
        : sendInterval_(sendInterval),
          checkInterval_(checkInterval) {
    if (sendInterval_ <= milliseconds(0)) {
        sendInterval_ = milliseconds(1000);
    }
    if (checkInterval_ < sendInterval_) {
        checkInterval_ = sendInterval_;
    }
}

milliseconds KeepAliveParameters::SendInterval() const {
    return sendInterval_;
}

milliseconds KeepAliveParameters::CheckInterval() const {
    return checkInterval_;
}

}    // Namesapce IO.
}    // Namespace K.
