#include <K/Core/Log.h>

#include <typeinfo>
#include <cstdio>
#include <K/Core/Interface.h>

using std::string;
using std::puts;

namespace K {
namespace Core {

void Log::Print(Level level, Interface *source, const std::function<std::string()> &generateLogLine) {
    (void)level;
    puts((string("[") + (source ? typeid(*source).name() : "") + "] " + generateLogLine()).c_str());
}

}    // Namespace Core.
}    // Namespace K.
