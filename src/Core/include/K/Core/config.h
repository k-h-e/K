#ifndef K_CORE_CONFIG_H_
#define K_CORE_CONFIG_H_

#define K_PLATFORM_SET 1
#ifdef __APPLE__
    #define K_PLATFORM_APPLE
#endif

#include <stdint.h>

namespace K {
namespace Core {

//! Used for spacer data members guarding against the "neighboring field" multithreading issue.
typedef uint64_t spacer_t;

}    // Namespace Core.
}    // Namespace K.


#endif    // K_CORE_CONFIG_H_
