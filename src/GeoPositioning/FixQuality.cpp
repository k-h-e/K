/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#include <K/GeoPositioning/FixQuality.h>

using std::string;
using std::to_string;

namespace K {
namespace GeoPositioning {

bool IsValue(FixQuality quality) {
    return    (quality == FixQuality::Differential)
           || (quality == FixQuality::Regular)
           || (quality == FixQuality::RtkFixed)
           || (quality == FixQuality::RtkFloating);
}

string ToString(FixQuality fixQuality) {
    switch (fixQuality) {
        case FixQuality::Regular:
            return "regular";
        case FixQuality::Differential:
            return "differential";
        case FixQuality::RtkFloating:
            return "rtk_floating";
        case FixQuality::RtkFixed:
            return "rtk_fixed";
        default:
            return string("? (") + to_string(static_cast<unsigned int>(fixQuality)) + ")";
    }
}

}    // Namespace GeoPositioning.
}    // Namespace K.
