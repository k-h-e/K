/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther.                                                                      //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#include <K/GeoPositioning/AltitudeComputer.h>

#include <K/Core/Log.h>

using std::to_string;
using K::Core::Log;

namespace K {
namespace GeoPositioning {

AltitudeComputer::AltitudeComputer(Format format, double correction)
        : format_{format},
          correction_{correction} {
    // Nop.
}

double AltitudeComputer::Compute(double altitudeAboveGeoid, double geoidalSeparation) const {
    if (format_ == Format::AboveWgs84Ellipsoid) {
        return altitudeAboveGeoid + geoidalSeparation + correction_;
    } else {
        return altitudeAboveGeoid + correction_;
    }
}

}    // Namespace GeoPositioning.
}
