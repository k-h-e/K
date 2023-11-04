/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's C++ Crossplatform Assets                                                                        ///
//    (C) Copyright Kai Hergenröther. All rights reserved.                                                 //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#include <K/IO/StreamSaver.h>

#include <K/IO/File.h>
#include <K/IO/StreamBuffer.h>

using std::make_shared;
using std::make_unique;
using std::string;
using K::Core::StreamInterface;
using K::IO::Path;

namespace K {
namespace IO {

StreamSaver::StreamSaver(const Path &fileName)
        : fileStream_(make_unique<StreamBuffer>(make_shared<File>(fileName, File::AccessMode::WriteOnly, true),
                                                File::AccessMode::WriteOnly, 4 * 1024)) {
    // Nop.
}

void StreamSaver::OnStreamData(int id, const void *data, int dataSize) {
    (void)id;
    if (fileStream_) {
        WriteItem(fileStream_.get(), data, dataSize);
    }
}

void StreamSaver::OnStreamEnteredErrorState(int id, StreamInterface::Error error) {
    (void)id;
    (void)error;
    fileStream_.reset();
}

}    // Namespace IO.
}    // Namespace K.
