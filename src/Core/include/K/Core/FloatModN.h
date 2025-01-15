/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_CORE_FLOATMODN_H_
#define K_CORE_FLOATMODN_H_

namespace K {
namespace Core {

//! Comparable to \ref IntModN, yet float-valued.
class FloatModN {
  public:
    //! Initializes the variable to the value of <c>0.0f</c>.
    /*!
     *  \param modulus
     *  Must be at least <c>2</c>.
     */
    FloatModN(int modulus);
    FloatModN()                                  = delete;
    FloatModN(const FloatModN &other)            = default;
    FloatModN &operator=(const FloatModN &other) = default;
    FloatModN(FloatModN &&other)                 = default;
    FloatModN &operator=(FloatModN &&other)      = default;
    ~FloatModN()                                 = default;

    //! Returns the (always positive) distance to the specified other FloatModN, along the shorter way "around the
    //! ring".
    float Distance(const FloatModN &other) {
        float distance      = value_ > other.value_ ? value_ - other.value_ : other.value_ - value_,
              otherDistance = (float)modulus_ - distance;
        return distance < otherDistance ? distance : otherDistance;
    }
    
    //! The specified number must lie in <c>[0, modulus)</c>.
    void SetValue(float number);
    //! Returns the current value, lying in <c>[0, modulus)</c>.
    float Value() const;
    //! Adds the specified number, wrapping around accordingly to the modulus.
    void Add(float number);
    //! Returns the distance to the specified other value, choosing the shorter way around the ring.
    /*!
     *  Note that the distance might be negative, depending on which way around the ring is shorter.
     *
     *  Both variables must have the same modulus.
     */
    float DistanceTo(const FloatModN &other) const;
    //! Changes the value in order to getting it closer to the specified other value, choosing the shorter way around
    //! the ring, and not exceeding the specified maximum step size.
    void MoveTo(const FloatModN &other, float maxStepSize);
    
  private:
    float value_;
    int   modulus_;
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_FLOATMODN_H_
