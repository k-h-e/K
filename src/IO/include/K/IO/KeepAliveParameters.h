/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's C++ Crossplatform Assets                                                                        ///
//    (C) Copyright Kai Hergenröther. All rights reserved.                                                 //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_IO_KEEPALIVEPARAMETERS_H_
#define K_IO_KEEPALIVEPARAMETERS_H_

#include <chrono>
#include <K/Core/Interface.h>

namespace K {
namespace IO {

//! Set of keep-alive parameters.
class KeepAliveParameters : public virtual Core::Interface {
  public:
    KeepAliveParameters(std::chrono::milliseconds sendInterval, std::chrono::milliseconds checkInterval);
    KeepAliveParameters()                                            = delete;
    KeepAliveParameters(const KeepAliveParameters &other)            = default;
    KeepAliveParameters &operator=(const KeepAliveParameters &other) = default;
    KeepAliveParameters(KeepAliveParameters &&other)                 = default;
    KeepAliveParameters &operator=(KeepAliveParameters &&other)      = default;

    std::chrono::milliseconds SendInterval() const;
    std::chrono::milliseconds CheckInterval() const;

  private:
    std::chrono::milliseconds sendInterval_;
    std::chrono::milliseconds checkInterval_;
};

}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_KEEPALIVEPARAMETERS_H_
