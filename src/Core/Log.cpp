////    ////
////   ////     K Crossplatform C++ Assets
////  ////      (C) Copyright Kai Hergenröther
//// ////
////////        - Core -
//// ////
////  ////
////   ////
////    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <K/Core/Log.h>

#include <cstdio>
#include <K/Core/Interface.h>
#include <K/Core/StringTools.h>

using std::string;
using std::puts;

namespace K {
namespace Core {

Log::Level Log::currentLevel = Log::Level::Debug;

void Log::Print(Level level, const Interface *source, const std::function<std::string()> &generateLogLine) {
    if (level >= currentLevel) {
        puts((string("[") + StringTools::GetCleanClassName(source) + "] " + generateLogLine()).c_str());
    }
}

}    // Namespace Core.
}    // Namespace K.
