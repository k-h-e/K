#ifndef K_CORE_NUMBERTOOLS_H_
#define K_CORE_NUMBERTOOLS_H_

namespace K {
namespace Core {

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
};

}    // Namespace Core.
}    // namespace K.

#endif    // K_CORE_NUMBERTOOLS_H_
