#ifndef K_GEOPOSITIONING_LOCALCOORDINATESYSTEM_H_
#define K_GEOPOSITIONING_LOCALCOORDINATESYSTEM_H_

#include <K/Core/Interface.h>
#include <K/GeoPositioning/GeoPosition.h>
#include <Vectoid/Core/Vector.h>

namespace K {
namespace GeoPositioning {

//! Local Cartesian coordiate system, centered around a point on the Earth's surface, with <c>y</c> axis direction
//! pointing up and <c>z</c> axis direction pointing south.
class LocalCoordinateSystem : public virtual K::Core::Interface {
  public:
    LocalCoordinateSystem();
    //! Creates a local coordinate system with the specified point on the Earth's surface as origin, and with <c>y</c>
    //! axis direction pointing up and <c>z</c> axis direction pointing south.
    LocalCoordinateSystem(const GeoPosition &origin);
    LocalCoordinateSystem(const LocalCoordinateSystem &other)            = default;
    LocalCoordinateSystem &operator=(const LocalCoordinateSystem &other) = default;
    LocalCoordinateSystem(LocalCoordinateSystem &&other)                 = default;
    LocalCoordinateSystem &operator=(LocalCoordinateSystem &&other)      = default;

    bool operator==(const LocalCoordinateSystem &other) const;
    //! Sets up a local coordinate system with the specified point on the Earth's surface as origin, and with <c>y</c>
    //! pointing up and <c>z</c> pointing south.
    void Reset(const GeoPosition &origin);
    //! Transforms the specified point on the Earth's surface to the local coordinate system.
    Vectoid::Core::Vector<float> ToLocal(const GeoPosition &position) const;
    //! Transforms the specified point in local coordinates to polar Earth coordinates.
    GeoPosition ToEarthPolar(const Vectoid::Core::Vector<float> &position) const;
    //! Returns the coordinate system origin in polar Earth coordinates.
    GeoPosition Origin() const;

  private:
    static const double earthRadius;

    static void ComputeAbsolute(const GeoPosition &polar, Vectoid::Core::Vector<double> *outAbsolute);

    GeoPosition                   originPolar_;
    Vectoid::Core::Vector<double> origin_;
    Vectoid::Core::Vector<double> x_;
    Vectoid::Core::Vector<double> y_;
    Vectoid::Core::Vector<double> z_;
};

}    // Namespace GeoPositioning.
}    // Namespace K.

#endif    // K_GEOPOSITIONING_LOCALCOORDINATESYSTEM_H_
