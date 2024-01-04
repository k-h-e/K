/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther.                                                                      //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_CORE_NUMBERTOOLS_H_
#define K_CORE_NUMBERTOOLS_H_

#include <cstdint>

namespace K {
namespace Core {

//! Tool functions dealing with numbers.
class NumberTools {
  public:
    static const double pi;

    template<typename T>
    static void Clamp(T *numba, T min, T max) {
        if (*numba < min) {
            *numba = min;
        }
        else if (*numba > max) {
            *numba = max;
        }
    }

    template<typename T>
    static void ClampMin(T *numba, T min) {
        if (*numba < min) {
            *numba = min;
        }
    }

    template<typename T>
    static void ClampMax(T *numba, T max) {
        if (*numba > max) {
            *numba = max;
        }
    }

    template<typename T>
    static void Swap(T *number, T *otherNumber) {
        T temporary = *number;
        *number = *otherNumber;
        *otherNumber = temporary;
    }

    static void ToggleEndian(uint16_t *number) {
        uint8_t *ptr = reinterpret_cast<uint8_t *>(number);
        uint8_t temp = ptr[0];
        ptr[0] = ptr[1];
        ptr[1] = temp;
    }

    static double DegToRad(double angleDeg);
    static double RadToDeg(double angleRad);
};

}    // Namespace Core.
}    // namespace K.

#endif    // K_CORE_NUMBERTOOLS_H_
