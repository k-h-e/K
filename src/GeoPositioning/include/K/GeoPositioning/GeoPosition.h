/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_GEOPOSITIONING_GEOPOSITION_H_
#define K_GEOPOSITIONING_GEOPOSITION_H_

#include <string>
#include <K/Core/SerializableInterface.h>

namespace K {
//! Geopositioning.
namespace GeoPositioning {

//! Position on Earth surface expressed as <c>(latitude, longitude)</c>.
class GeoPosition : public K::Core::SerializableInterface {
  public:
    GeoPosition();
    GeoPosition(double latitude, double longitude);
    GeoPosition(const GeoPosition &other)            = default;
    GeoPosition &operator=(const GeoPosition &other) = default;
    GeoPosition(GeoPosition &&other)                 = default;
    GeoPosition &operator=(GeoPosition &&other)      = default;

    bool operator==(const GeoPosition &other) const;
    double Latitude() const;
    double Longitude() const;
    std::string ToString() const;

    void Serialize(K::Core::BlockingOutStreamInterface &stream) const override;
    void Deserialize(K::Core::BlockingInStreamInterface &stream) override;

  private:
    double latitude_;
    double longitude_;
};

}    // Namespace GeoPositioning.
}    // Namespace K.

#endif    // K_GEOPOSITIONING_GEOPOSITION_H_
