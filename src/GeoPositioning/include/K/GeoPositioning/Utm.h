/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's C++ Crossplatform Assets                                                                        ///
//    (C) Copyright Kai Hergenröther. All rights reserved.                                                 //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_GEOPOSITIONING_UTM_H_
#define K_GEOPOSITIONING_UTM_H_

#include <K/Core/Interface.h>

namespace K {
namespace GeoPositioning {
    class GeoPosition;
}
}

namespace K {
namespace GeoPositioning {

//! Converts geo positions to and from UTM.
/*!
 *  Taken from https://github.com/Turbo87/utm.
 */
class Utm : public K::Core::Interface {
  public:
    Utm();
    Utm(const Utm &other)            = default;
    Utm &operator=(const Utm &other) = default;
    Utm(Utm &&other)                 = default;
    Utm &operator=(Utm &&other)      = default;
    ~Utm()                           = default;

    bool FromUtm(double easting, double northing, int zoneNumber, char zoneLetter, GeoPosition *outPosition);

  private:
    static const char *zoneLetters;

    void ComputeConstants();
    double CentralLongitude(int zoneNumber);

    double k0_;
    double e_;
    double eP2_;
    double m1_;
    double m2_;
    double m3_;
    double m4_;
    double p2_;
    double p3_;
    double p4_;
    double p5_;
    double r_;
};

}    // Namespace GeoPositioning.
}    // Namespace K.

#endif    // K_GEOPOSITIONING_UTM_H_
