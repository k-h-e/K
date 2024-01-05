/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_GEOPOSITIONING_ALTITUDECOMPUTER_H_
#define K_GEOPOSITIONING_ALTITUDECOMPUTER_H_

#include <K/Core/Interface.h>

namespace K {
namespace GeoPositioning {

//! Computes geopositioning altitudes according to a specific configuration.
class AltitudeComputer : public K::Core::Interface {
  public:
    enum class Format { AboveWgs84Ellipsoid,
                        AboveGeoid           };

    AltitudeComputer(Format format, double correction);
    AltitudeComputer()                                         = delete;
    AltitudeComputer(const AltitudeComputer &other)            = default;
    AltitudeComputer &operator=(const AltitudeComputer &other) = default;
    AltitudeComputer(AltitudeComputer &&other)                 = default;
    AltitudeComputer &operator=(AltitudeComputer &&other)      = default;
    ~AltitudeComputer()                                        = default;

    //! Computes the altitude as configured from the specified input data.
    double Compute(double altitudeAboveGeoid, double geoidalSeparation) const;

  private:
    Format format_;
    double correction_;
};

}    // Namespace GeoPositioning.
}    // Namespace K.

#endif    // K_GEOPOSITIONING_ALTITUDECOMPUTER_H_
