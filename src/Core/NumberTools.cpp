#include <K/Core/NumberTools.h>

namespace K {
namespace Core {

const double NumberTools::pi = 3.141592654;

void NumberTools::Clamp(int *numba, int min, int max) {
    if (*numba < min) {
        *numba = min;
    }
    else if (*numba > max) {
        *numba = max;
    }
}

void NumberTools::Clamp(float *numba, float min, float max) {
    if (*numba < min) {
        *numba = min;
    }
    else if (*numba > max) {
        *numba = max;
    }
}

void NumberTools::ClampMin(int *numba, int min) {
    if (*numba < min) {
        *numba = min;
    }
}

void NumberTools::ClampMax(int *numba, int max) {
    if (*numba > max) {
        *numba = max;
    }
}

void NumberTools::ClampMin(float *numba, float min) {
    if (*numba < min) {
        *numba = min;
    }
}

void NumberTools::ClampMax(float *numba, float max) {
    if (*numba > max) {
        *numba = max;
    }
}

}    // Namespace Core.
}    // namespace K.
