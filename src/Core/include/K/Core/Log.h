#ifndef K_CORE_LOG_H_
#define K_CORE_LOG_H_

#include <string>
#include <functional>

namespace K {
namespace Core {

class Interface;

//! Logging mechanism.
class Log {
  public:
    enum class Level { DebugDebug,
                       Debug,
                       Info,
                       Warning,
                       Error       };

    //! Thread-safe.
    static void Print(Level level, Interface *source, const std::function<std::string()> &generateLogLine);

  private:
    static uint64_t spacer;
    static Level    currentLevel;    // Will be using this without thread protection for now, okay.
    static uint64_t spacer2;
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_LOG_H_
