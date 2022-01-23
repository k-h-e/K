////    ////
////   ////     K Crossplatform C++ Assets
////  ////      (C) Copyright Kai Hergenröther
//// ////
////////        - Core -
//// ////
////  ////
////   ////
////    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <K/Core/FloatModN.h>

#include <cassert>
#include <K/Core/NumberTools.h>

namespace K {
namespace Core {

FloatModN::FloatModN(int modulus)
    : value_(0.0f),
      modulus_(modulus) {
    assert(modulus >= 2);
}

void FloatModN::SetValue(float number) {
    NumberTools::Clamp(&number, 0.0f, static_cast<float>(modulus_));
    value_ = (number < modulus_) ? number : 0.0f;
}

float FloatModN::Value() const {
    return value_;
}

void FloatModN::Add(float number) {
    value_ += number;
    while (value_ < 0.0f) {
        value_ += modulus_;
    }
    while (value_ >= modulus_) {
        value_ -= modulus_;
    }
}

float FloatModN::DistanceTo(const FloatModN &other) const {
    assert(modulus_ == other.modulus_);
    
    bool distanceIsNegative = false;
    float distance = other.value_ - value_;
    if (distance < 0.0f) {
        distance = -distance;
        distanceIsNegative = true;
    }
    if (distance > (float)modulus_ / 2.0f) {
        distance = (float)modulus_ - distance;
        distanceIsNegative = !distanceIsNegative;
    }
    return distanceIsNegative ? -distance : distance;
}

void FloatModN::MoveTo(const FloatModN &other, float maxStepSize) {
    bool distanceIsNegative = false;
    float distance = DistanceTo(other);
    if (distance < 0.0f) {
        distance           = -distance;
        distanceIsNegative = true;
    }
    float stepSize = distance;
    NumberTools::Clamp(&stepSize, 0.0f, maxStepSize);
    Add(distanceIsNegative ? -stepSize : stepSize);
}

}    // Namespace Core.
}    // Namespace K.
