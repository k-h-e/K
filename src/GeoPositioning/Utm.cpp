/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's C++ Crossplatform Assets                                                                        ///
//    (C) Copyright Kai Hergenröther.                                                                      //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#include <K/GeoPositioning/Utm.h>

#include <cmath>
#include <K/Core/NumberTools.h>
#include <K/GeoPositioning/GeoPosition.h>

using std::cos;
using std::sin;
using std::sqrt;
using K::Core::NumberTools;

namespace K {
namespace GeoPositioning {

const char *Utm::zoneLetters = "CDEFGHJKLMNPQRSTUVWXX";

Utm::Utm() {
    ComputeConstants();
}

bool Utm::FromUtm(double easting, double northing, int zoneNumber, char zoneLetter, GeoPosition *outPosition) {
    if ((easting >= 100000.0) && (easting < 1000000.0) && (northing >= 0.0) && (northing <= 10000000.0)
            && (zoneNumber >= 1) && (zoneNumber <= 60) && (zoneLetter >= 'C') && (zoneLetter <= 'X')) {
        bool   northern = (zoneLetter >= 'N');

        double x        = easting - 500000.0;
        double y        = northing;
        if (!northern) {
            y -= 10000000.0;
        }

        double m  = y / k0_;
        double mu = m / (r_ * m1_);

        double pRad = mu + p2_*sin(2.0 * mu)
                         + p3_*sin(4.0 * mu)
                         + p4_*sin(6.0 * mu)
                         + p5_*sin(8.0 * mu);

        double pSin  = sin(pRad);
        double pSin2 = pSin * pSin;
        double pCos  = cos(pRad);
        double pCos2 = pCos * pCos;

        double pTan  = pSin / pCos;
        double pTan2 = pTan * pTan;
        double pTan4 = pTan2 * pTan2;

        double epSin     = 1.0 - e_*pSin2;
        double epSinSqrt = sqrt(1.0 - e_*pSin2);

        double n = r_ / epSinSqrt;
        double r = (1.0 - e_) / epSin;

        double c  = eP2_ * pCos2;
        double c2 = c * c;

        double d  = x / (n * k0_);
        double d2 = d * d;
        double d3 = d2 * d;
        double d4 = d3 * d;
        double d5 = d4 * d;
        double d6 = d5 * d;

        double latitude = (pRad - (pTan / r) *
                           (d2/2.0 -
                            d4 / 24.0 * (5.0 + 3.0*pTan2 + 10.0*c - 4.0*c2 - 9.0 * eP2_)) +
                            d6 / 720.0 * (61.0 + 90.0*pTan2 + 298.0*c + 45.0* pTan4 - 252.0*eP2_ - 3.0*c2));
        double longitude = (d -
                            d3 / 6.0 * (1.0 + 2.0*pTan2 + c) +
                            d5 / 120.0 * (5.0 - 2.0*c + 28.0*pTan2 - 3.0*c2 + 8.0*eP2_ + 24.0*pTan4)) / pCos;
        longitude += NumberTools::DegToRad(CentralLongitude(zoneNumber));
        while (longitude < -NumberTools::pi) {
            longitude += 2.0 * NumberTools::pi;
        }
        while (longitude > NumberTools::pi) {
            longitude -= 2.0 * NumberTools::pi;
        }

        *outPosition = GeoPosition(NumberTools::RadToDeg(latitude), NumberTools::RadToDeg(longitude));
        return true;
    }

    return false;
}

void Utm::ComputeConstants() {
    k0_ = 0.9996;

    e_        = 0.00669438;
    double e2 = e_ * e_;
    double e3 = e2 * e_;
    eP2_      = e_ / (1.0 - e_);

    double sqrtE = sqrt(1.0 - e_);
    double ee    = (1.0 - sqrtE) / (1.0 + sqrtE);
    double ee2   = ee * ee;
    double ee3   = ee2 * ee;
    double ee4   = ee3 * ee;
    double ee5   = ee4 * ee;

    m1_ = 1.0 - e_/4.0 - 3.0*e2/64.0 - 5.0*e3/256.0;
    m2_ = 3.0*e_/8.0 + 3.0*e2/32.0 + 45.0*e3/1024.0;
    m3_ = 15.0*e2/256.0 + 45.0*e3/1024.0;
    m4_ = 35.0*e3/3072.0;

    p2_ = 3.0/2.0*ee - 27.0/32.0*ee3 + 269.0/512.0*ee5;
    p3_ = 21.0/16.0*ee2 - 55.0/32.0*ee4;
    p4_ = 151.0/96.0*ee3 - 417.0/128.0*ee5;
    p5_ = 1097.0/512.0*ee4;

    r_ = 6378137.0;
}

double Utm::CentralLongitude(int zoneNumber) {
    return 6.0*static_cast<double>(zoneNumber - 1) - 180.0 + 3.0;
}

}    // Namespace GeoPositioning.
}    // Namespace K.
