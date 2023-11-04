/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's C++ Crossplatform Assets                                                                        ///
//    (C) Copyright Kai Hergenröther. All rights reserved.                                                 //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_GEOPOSITIONING_FIXQUALITY_H_
#define K_GEOPOSITIONING_FIXQUALITY_H_

#include <cstdint>
#include <string>

namespace K {
namespace GeoPositioning {

//! Classifies the quality of a given geopositioning fix.
enum class FixQuality : uint32_t  {
    Regular,
    Differential,
    RtkFloating,
    RtkFixed
};

//! Tells whether the specified quality is a valid enumeration value.
bool IsValue(FixQuality quality);

//! Returns a string representation for the specified fix quality.
std::string ToString(FixQuality fixQuality);

}    // Namespace GeoPositioning.
}    // Namespace K.

#endif    // K_GEOPOSITIONING_FIXQUALITY_H_
