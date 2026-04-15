/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#include <K/Core/Log.h>

#if defined(K_PLATFORM_ANDROID)
#include <android/log.h>
#endif

#include <cstdio>

#include <K/Core/Interface.h>
#include <K/Core/StringTools.h>

using std::string;
using std::puts;

namespace K {
namespace Core {

Log::Level Log::currentLevel = Log::Level::Debug;

#if defined(K_PLATFORM_MAC) || defined(K_PLATFORM_IOS) || defined(K_PLATFORM_LINUX)
void Log::Print(Level level, const Interface *source, const std::function<std::string()> &generateLogLine) {
    if (level >= currentLevel) {
        if (source) {
            puts((string("[") + StringTools::GetCleanClassName(*source) + "] " + generateLogLine()).c_str());
        } else {
            puts(generateLogLine().c_str());
        }
    }
}
#elif defined(K_PLATFORM_ANDROID)
void Log::Print(Level level, const Interface *source, const std::function<std::string()> &generateLogLine) {
    if (level >= currentLevel) {
        if (source) {
            __android_log_print(
                ANDROID_LOG_DEBUG, StringTools::GetCleanClassName(*source, 2).c_str(),
                "%s", generateLogLine().c_str());
        } else {
            __android_log_print(ANDROID_LOG_DEBUG, "native", "%s", generateLogLine().c_str());
        }
    }
}
#else
#error Unknown platform.
#endif

}    // Namespace Core.
}    // Namespace K.
