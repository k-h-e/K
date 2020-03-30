#ifndef K_CORE_LOG_H_
#define K_CORE_LOG_H_

#include <string>
#include <functional>

namespace K {
namespace Core {

class Interface;

//! Logging mechanism.
/*!
 *  \ingroup Core
 */
class Log {
  public:
    enum class Level { Debug,
                       Info,
                       Warning,
                       Critical };

    //! Thread-safe.
    static void Print(Level level, Interface *source, const std::function<std::string()> &generateLogLine);
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_LOG_H_
