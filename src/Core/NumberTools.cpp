/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's C++ Crossplatform Assets                                                                        ///
//    (C) Copyright Kai Hergenröther. All rights reserved.                                                 //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#include <K/Core/NumberTools.h>

namespace K {
namespace Core {

const double NumberTools::pi = 3.141592654;

double NumberTools::DegToRad(double angleDeg) {
    return angleDeg * pi / 180.0;
}

double NumberTools::RadToDeg(double angleRad) {
    return angleRad * 180.0 / pi;
}

}    // Namespace Core.
}    // namespace K.
