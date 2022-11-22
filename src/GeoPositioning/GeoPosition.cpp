#include <K/GeoPositioning/GeoPosition.h>

#include <cmath>
#include <K/Core/IOOperations.h>
#include <K/Core/NumberTools.h>

using std::isfinite;
using std::string;
using K::Core::BinaryReaderInterface;
using K::Core::BinaryWriterInterface;
using K::Core::NumberTools;

namespace K {
namespace GeoPositioning {

GeoPosition::GeoPosition()
        : latitude_(0.0),
          longitude_(0.0) {
    // Nop.
}

GeoPosition::GeoPosition(double latitude, double longitude)
        : latitude_(latitude),
          longitude_(longitude) {
    if (!isfinite(latitude_)) {
        latitude_ = 0.0;
    }
    if (!isfinite(longitude_)) {
        longitude_ = 0.0;
    }

    NumberTools::Clamp(&latitude_, -90.0, 90.0);

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
    std::snprintf(latitudeText, 40, "%.8f", latitude_);
    std::snprintf(longitudeText, 40, "%.8f", longitude_);
    return string("(lat=") + latitudeText + ", lng=" + longitudeText + ")";
}

void GeoPosition::Serialize(BinaryWriterInterface *writer) const {
    (*writer) << latitude_;
    (*writer) << longitude_;
}

void GeoPosition::Deserialize(BinaryReaderInterface *reader) {
    (*reader) >> latitude_;
    (*reader) >> longitude_;
}

}    // Namespace GeoPositioning.
}    // Namespace K.
