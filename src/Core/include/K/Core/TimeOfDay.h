////    ////
////   ////     K Crossplatform C++ Assets
////  ////      (C) Copyright Kai Hergenröther
//// ////
////////        - Core -
//// ////
////  ////
////   ////
////    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef K_CORE_TIMEOFDAY_H_
#define K_CORE_TIMEOFDAY_H_

#include <string>
#include <K/Core/SerializableInterface.h>

namespace K {
namespace Core {

//! Holds a time of day in 24h format.
class TimeOfDay : public virtual SerializableInterface {
  public:
    TimeOfDay(int hours, int minutes, int seconds, int milliSeconds);
    //! Initializes to midnight.
    TimeOfDay();
    TimeOfDay(const TimeOfDay &other)            = default;
    TimeOfDay &operator=(const TimeOfDay &other) = default;
    TimeOfDay(TimeOfDay &&other)                 = default;
    TimeOfDay &operator=(TimeOfDay &&other)      = default;

    std::string ToString() const;

    void Serialize(BlockingOutStreamInterface *stream) const override;
    void Deserialize(BlockingInStreamInterface *stream) override;

    //! Returns the current time of day in UTC.
    static TimeOfDay NowUtc();

  private:
    int hours_;
    int minutes_;
    int seconds_;
    int milliSeconds_;
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_TIMEOFDAY_H_
