#include <K/GeoPositioning/NmeaDevice.h>

#include <sstream>
#include <cstring>
#include <cassert>
#include <iomanip>
#include <K/Core/ConnectionStreamInterface.h>
#include <K/Core/Log.h>

using std::shared_ptr;
using std::to_string;
using K::Core::ConnectionStreamInterface;
using K::Core::Log;

namespace K {
namespace GeoPositioning {

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
        auto parser{make_shared<NmeaParser>(handler, 0)};
        if (connection_->Register(parser, 0)) {
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
            Log::Print(Log::Level::Warning, this, [&]{
                return "connection in error state, error=" + to_string(static_cast<int>(connection_->StreamError()));
            });
            error_ = true;
        }
    }
    return error_;
}

}    // Namespace GeoPositioning.
}    // Namespace K.

