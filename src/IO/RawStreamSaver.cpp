/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther.                                                                      //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#include <K/IO/RawStreamSaver.h>

#include <K/IO/File.h>
#include <K/IO/StreamBuffer.h>

using std::make_shared;
using std::make_unique;
using std::string;
using K::Core::StreamInterface;
using K::IO::Path;

namespace K {
namespace IO {

RawStreamSaver::RawStreamSaver(const Path &fileName)
        : fileStream_(make_unique<StreamBuffer>(make_shared<File>(fileName, File::AccessMode::WriteOnly, true),
                                                File::AccessMode::WriteOnly, 4 * 1024)) {
    // Nop.
}

void RawStreamSaver::OnRawStreamData(const void *data, int dataSize) {
    if (fileStream_) {
        WriteItem(fileStream_.get(), data, dataSize);
    }
}

void RawStreamSaver::OnStreamError(StreamInterface::Error error) {
    (void)error;
    fileStream_.reset();
}

}    // Namespace IO.
}    // Namespace K.
