/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther.                                                                      //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#include <K/GeoPositioning/LocalCoordinateSystem.h>

#include <K/Core/Log.h>
#include <K/Core/NumberTools.h>
#include <Vectoid/Core/Transform.h>
#include <Vectoid/Core/Range.h>

using std::to_string;
using K::Core::Log;
using K::Core::NumberTools;
using Vectoid::Core::Axis;
using Vectoid::Core::Vector;
using Vectoid::Core::Transform;
using Vectoid::Core::Range;

namespace K {
namespace GeoPositioning {

const double LocalCoordinateSystem::earthRadius = 6371000.0;

LocalCoordinateSystem::LocalCoordinateSystem() {
    Reset(GeoPosition(0.0, 0.0));
}

LocalCoordinateSystem::LocalCoordinateSystem(const GeoPosition &origin) {
    Reset(origin);
}

bool LocalCoordinateSystem::operator==(const LocalCoordinateSystem &other) const {
    return (originPolar_ == other.originPolar_);
}

void LocalCoordinateSystem::Reset(const GeoPosition &origin) {
    originPolar_ = origin;
    ComputeAbsolute(originPolar_, &origin_);

    y_ = origin_;
    y_.Normalize();

    Vector<double> straightNorth = Vector<double>(0.0, earthRadius, 0.0) - origin_;
    straightNorth.Normalize();
    x_ = CrossProduct(straightNorth, y_);
    x_.Normalize();

    z_ = CrossProduct(x_, y_);
    z_.Normalize();
}

Vector<float> LocalCoordinateSystem::ToLocal(const GeoPosition &position) const {
    Vector<double> pointAbsolute;
    ComputeAbsolute(position, &pointAbsolute);
    Vector<double> delta = pointAbsolute - origin_;
    return Vector<float>(static_cast<float>(DotProduct(delta, x_)),
                         static_cast<float>(DotProduct(delta, y_)),
                         static_cast<float>(DotProduct(delta, z_)));
}

GeoPosition LocalCoordinateSystem::ToEarthPolar(const Vector<float> &position) const {
    Vector<double> point = origin_ + static_cast<double>(position.x)*x_
                                   + static_cast<double>(position.y)*y_
                                   + static_cast<double>(position.z)*z_;
    point.Normalize();
    Vector<double> pointXZ = point;
    pointXZ.y = 0.0;
    double radiusXZ = pointXZ.Length();

    double arg = point.y;
    NumberTools::Clamp(&arg, -1.0, 1.0);
    double latitude = std::asin(arg) / NumberTools::pi * 180.0;

    double longitude;
    arg = point.x / radiusXZ;
    if (std::isfinite(arg)) {
        longitude = std::asin(arg) / NumberTools::pi * 180.0;
        if (point.z < 0.0) {
            longitude = 180.0 - longitude;
            if (longitude < -180.0) {
                longitude += 360.0;
            }
            else if (longitude > 180.0) {
                longitude -= 360.0;
            }
        }
    }
    else {
        longitude = 0.0;
    }

    return GeoPosition(latitude, longitude);
}

GeoPosition LocalCoordinateSystem::Origin() const {
    return originPolar_;
}

void LocalCoordinateSystem::ComputeAbsolute(const GeoPosition &polar, Vector<double> *outAbsolute) {
    outAbsolute->Set(0.0, 0.0, earthRadius);
    Transform<double>(Axis::X, -polar.Latitude()).ApplyTo(outAbsolute);
    Transform<double>(Axis::Y, polar.Longitude()).ApplyTo(outAbsolute);
}

}    // Namespace GeoPositioning.
}    // Namespace K.
