/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther.                                                                      //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#include <K/Core/BlockingInStreamInterface.h>

#include <cassert>
#include <K/Core/StringTools.h>

using std::string;

namespace K {
namespace Core {

void ReadItem(BlockingInStreamInterface *stream, void *item, int itemSize) {
    assert(itemSize > 0);
    uint8_t *dest   = static_cast<uint8_t *>(item);
    int     numLeft = itemSize;
    while (numLeft) {
        int numRead = stream->ReadBlocking(dest, numLeft);
        if (numRead) {
            dest    += numRead;
            numLeft -= numRead;
        } else {
            assert (stream->ErrorState());
            return;
        }
    }
}

void Skip(BlockingInStreamInterface *stream, int numBytes) {
    uint8_t value;
    while ((numBytes > 0) && !stream->ErrorState()) {
        (*stream) >> value;
        --numBytes;
    }
}

BlockingInStreamInterface &operator>>(BlockingInStreamInterface &stream, string &outValue) {
    StringTools::Deserialize(&outValue, &stream);
    return stream;
}

}    // Namespace Core.
}    // namespace K.
