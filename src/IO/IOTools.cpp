/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's C++ Crossplatform Assets                                                                        ///
//    (C) Copyright Kai Hergenröther.                                                                      //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#include <K/IO/IOTools.h>

#include <unistd.h>
#include <cerrno>
#include <K/Core/Log.h>

using std::string;
using std::to_string;
using K::Core::Log;

namespace K {
namespace IO {

const char *IOTools::whiteSpace = " \t\r\n";

bool IOTools::CloseFileDescriptor(int fd, Core::Interface *loggingObject) {
    while (true) {
        if (!close(fd)) {
            return true;
        }
        else {
            if (errno != EINTR) {
                Log::Print(Log::Level::Error, loggingObject, [&]{
                    return "error while closing fd " + to_string(fd) + ",  errno=" + to_string(errno);
                });
                return false;
            }
        }
    }
}

}    // Namespace IO.
}    // namespace K.
