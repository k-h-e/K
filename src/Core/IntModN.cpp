/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's C++ Crossplatform Assets                                                                        ///
//    (C) Copyright Kai Hergenröther.                                                                      //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#include <K/Core/IntModN.h>

#include <K/Core/NumberTools.h>

namespace K {
namespace Core {

IntModN::IntModN(int modulus, int offset) {
    if (modulus < 2)
        modulus = 2;
    number_  = 0;
    modulus_ = modulus;
    offset_  = offset;
}

bool IntModN::operator==(const IntModN &other) const {
    return    (number_  == other.number_)
           && (modulus_ == other.modulus_)
           && (offset_  == other.offset_);
}

bool IntModN::operator!=(const IntModN &other) const {
    return !(*this == other);
}

bool IntModN::operator<(const IntModN &other) const {
    int delta = other - *this;
    if (delta == -1)
        return false;
    else
        return (delta != 0) && (delta <= modulus_/2);
}

bool IntModN::operator>(const IntModN &other) const {
    return other < *this;
}

IntModN &IntModN::operator++() {
    number_++;
    if (number_ >= modulus_)
        number_ = 0;
    return *this;
}

IntModN &IntModN::operator--() {
    number_--;
    if (number_ < 0)
        number_ = modulus_ - 1;
    return *this;
}

int IntModN::operator-(const IntModN &other) const {
    if ((modulus_ != other.modulus_) || (offset_ != other.offset_))
        return -1;
    else
        return (other.number_ <= number_) ? number_ - other.number_
                                          : (modulus_ - other.number_) + number_;
}

IntModN IntModN::operator+(int number) const {
    while (number < 0)
        number += modulus_;
    while (number >= modulus_)
        number -= modulus_;
    if (number == 0)
        return *this;
    
    IntModN result = *this;
    int numLeft = modulus_ - 1 - number_;
    if (numLeft >= number)
        result.number_ += number;
    else
        result.number_ = number - numLeft - 1;
    return result;
}

void IntModN::SetValue(int number) {
    number -= offset_;
    NumberTools::Clamp(&number, 0, modulus_ - 1);
    number_ = number;
}

int IntModN::ToInt() const {
    return number_ + offset_;
}

bool IntModN::IncrementWillWrap() const {
    return number_ == modulus_ - 1;
}

}    // Namespace Core.
}    // Namespace K.
