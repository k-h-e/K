/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

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
        if (source) {
            puts((string("[") + StringTools::GetCleanClassName(*source) + "] " + generateLogLine()).c_str());
        } else {
            puts(generateLogLine().c_str());
        }
    }
}

}    // Namespace Core.
}    // Namespace K.
