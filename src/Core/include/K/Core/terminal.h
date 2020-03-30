#ifndef K_CORE_TERMINAL_H_
#define K_CORE_TERMINAL_H_

#include <cstdlib>
#include <string>

namespace K {
namespace Core {

//! Central place to handle a terminal error condition.
inline void HandleTerminalCondition(const std::string &message) {
    std::printf("*** terminal condition: %s\n", message.c_str());
    std::exit(1);
}

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_TERMINAL_H_
