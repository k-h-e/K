/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther.                                                                      //  //
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
 *  Derived from code by Tobias Bieniek, published on GitHub under the MIT license:
 *
 *  Copyright (c) 2012-2017 Tobias Bieniek <Tobias.Bieniek@gmx.de>
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
 *  documentation files (the "Software"), to deal in the Software without restriction, including without limitation the
 *  rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
 *  permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the
 *  Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 *  WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS
 *  OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 *  OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
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
