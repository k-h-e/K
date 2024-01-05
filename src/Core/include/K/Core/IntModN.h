/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_CORE_INTMODN_H_
#define K_CORE_INTMODN_H_

namespace K {
namespace Core {

//! Integer variable representing a value from <c>[0, n - 1]</c> for a given modulus <c>n</c>, where this set is
//! interpreted as a ring-like structure.
/*!
 *  For example, incrementing <c>n - 1</c> by one will yield <c>0</c>. Furthermore, the whole set can be offset by a
 *  given value <c>offset</c>, thus becoming <c>[offset, offset + n - 1]</c>, with <c>(offset + n - 1) + 1 = offset</c>.
 */
class IntModN {
  public:
    //! Initializes the variable to the value of <c>offset</c>.
    /*!
     *  \param modulus
     *  Must be at least <c>2</c>.
     *
     *  \param offset
     *  If non-<c>0</c>, the number will not "rotate" over <c>[0, modulus - 1]</c>, but instead over
     *  <c>[offset, offset + modulus - 1]</c>.
     */
    IntModN(int modulus, int offset);
    IntModN()                                = delete;
    IntModN(const IntModN &other)            = default;
    IntModN &operator=(const IntModN &other) = default;
    IntModN(IntModN &&other)                 = default;
    IntModN &operator=(IntModN &&other)      = default;
    
    //! Well...
    bool operator==(const IntModN &other) const;
    //! Well...
    bool operator!=(const IntModN &other) const;
    //! Note that this relation has a special meaning so that it is applicable on a ring: <c>a</c> is smaller than
    //! <c>b</c> iff the ascending delta from <c>a</c> to <c>b</c> is positive but smaller (or equal) than half the
    //! modulus. The operands must have identical modulus and offset, otherwise <c>false</c> is returned.
    bool operator<(const IntModN &other) const;
    //! Compare operator<().
    bool operator>(const IntModN &other) const;
    //! Increment operator, wrapping back to <c>offset</c> when called for <c>offset + modulus - 1</c>.
    IntModN &operator++();
    //! Decrement operator, wrapping to <c>offset + modulus - 1</c> when called for <c>offset</c>.
    IntModN &operator--();
    //! <c>a - b</c> returns the (always non-negative) ascending distance from <c>b</c> to <c>a</c>, or <c>-1</c> in
    //! case <c>a</c> and <c>b</c> have incompatible modulus and offset parameters.
    int operator-(const IntModN &other) const;
    //! Adds the specified integer, which is allowed to be negative.
    IntModN operator+(int number) const;
    
    //! The specified number must lie in <c>[offset, offset + modulus - 1]</c>.
    void SetValue(int number);
    //! Returns the current value, lying in <c>[offset, offset + modulus - 1]</c>.
    int ToInt() const;
    //! Tells whether another increment by one will cause the current value to "wrap" down back to <c>offset</c>.
    bool IncrementWillWrap() const;
    
  private:
    int number_;
    int modulus_;
    int offset_;
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_INTMODN_H_

