/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#include <K/GeoPositioning/GeoPosition.h>

#include <cmath>

#include <K/Core/BlockingInStreamInterface.h>
#include <K/Core/BlockingOutStreamInterface.h>
#include <K/Core/NumberTools.h>

using std::isfinite;
using std::snprintf;
using std::string;
using K::Core::BlockingInStreamInterface;
using K::Core::BlockingOutStreamInterface;
using K::Core::NumberTools;

namespace K {
namespace GeoPositioning {

GeoPosition::GeoPosition()
        : latitude_{0.0},
          longitude_{0.0} {
    // Nop.
}

GeoPosition::GeoPosition(double latitude, double longitude)
        : latitude_{latitude},
          longitude_{longitude} {
    if (!isfinite(latitude_)) {
        latitude_ = 0.0;
    }
    if (!isfinite(longitude_)) {
        longitude_ = 0.0;
    }

    NumberTools::Clamp(latitude_, -90.0, 90.0);

    while (longitude_ < -180.0) {
        longitude_ += 360.0;
    }
    while (longitude_ > 180.0) {
        longitude_ -= 360.0;
    }
}

bool GeoPosition::operator==(const GeoPosition &other) const {
    return (latitude_ == other.latitude_) && (longitude_ == other.longitude_);
}

double GeoPosition::Latitude() const {
    return latitude_;
}

double GeoPosition::Longitude() const {
    return longitude_;
}

string GeoPosition::ToString() const {
    char latitudeText[40];
    char longitudeText[40];
    snprintf(latitudeText, 40, "%.8f", latitude_);
    snprintf(longitudeText, 40, "%.8f", longitude_);
    return string("(lat=") + latitudeText + ", lng=" + longitudeText + ")";
}

bool GeoPosition::DeserializeAndValidate(BlockingInStreamInterface &stream) {
    Deserialize(stream);
    if (!stream.ErrorState()) {
        if (isfinite(latitude_) && (latitude_ >= -90.0) && (latitude_ <= 90.0)
                && isfinite(longitude_) && (longitude_ >= -180.0) && (longitude_ <= 180.0)) {
            return true;
        }
    }

    *this = GeoPosition();
    return false;
}

void GeoPosition::Serialize(BlockingOutStreamInterface &stream) const {
    stream << latitude_;
    stream << longitude_;
}

void GeoPosition::Deserialize(BlockingInStreamInterface &stream) {
    stream >> latitude_;
    stream >> longitude_;
}

}    // Namespace GeoPositioning.
}    // Namespace K.
