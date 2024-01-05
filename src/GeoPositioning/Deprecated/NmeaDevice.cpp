/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#include <K/GeoPositioning/Deprecated/NmeaDevice.h>

#include <cstring>
#include <cassert>
#include <iomanip>
#include <sstream>

#include <K/Core/Log.h>
#include <K/Core/Deprecated/ConnectionStreamInterface.h>

using std::optional;
using std::shared_ptr;
using std::to_string;

using K::Core::Log;
using K::Core::StreamInterface;
using K::Core::Deprecated::ConnectionStreamInterface;

namespace K {
namespace GeoPositioning {
namespace Deprecated {

NmeaDevice::NmeaDevice(const shared_ptr<ConnectionStreamInterface> &connection)
        : connection_{connection},
          error_{false} {
    // Nop.
}

NmeaDevice::~NmeaDevice() {
    if (handler_) {
        Unregister(handler_);
    }
}

bool NmeaDevice::Register(const shared_ptr<NmeaMessageHandlerInterface> &handler) {
    assert(handler);

    if (handler_) {
        Unregister(handler_);
    }

    if (!error_) {
        auto parser{make_shared<NmeaParser>(handler)};
        if (connection_->Register(parser)) {
            parser_  = parser;
            handler_ = handler;
        } else {
            error_ = true;
        }
    }

    return !error_;
}

void NmeaDevice::Unregister(const std::shared_ptr<NmeaMessageHandlerInterface> &handler) {
    assert(handler);
    if (handler == handler_) {
        connection_->Unregister(parser_);
        parser_.reset();
        handler_.reset();
    }
}

bool NmeaDevice::Write(const NmeaMessage &message) {
    if (!error_) {
        message.WriteTo(connection_.get());
        if (!connection_->ErrorState()) {
            return true;
        }
    }

    error_ = true;
    return false;
}

bool NmeaDevice::ErrorState() const {
    if (!error_) {
        if (connection_->ErrorState()) {
            optional<StreamInterface::Error> error = connection_->StreamError();
            assert (error.has_value());
            Log::Print(Log::Level::Warning, this, [&]{
                return "connection in error state, error=" + to_string(static_cast<int>(*error));
            });
            error_ = true;
        }
    }
    return error_;
}

}    // Namespace Deprecated.
}    // Namespace GeoPositioning.
}    // Namespace K.

