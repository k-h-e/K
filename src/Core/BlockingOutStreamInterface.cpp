/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#include <K/Core/BlockingOutStreamInterface.h>

#include <cassert>
#include <K/Core/StringTools.h>

using std::string;

namespace K {
namespace Core {

void WriteItem(BlockingOutStreamInterface *stream, const void *item, int itemSize) {
    assert(itemSize > 0);
    const uint8_t *source = static_cast<const uint8_t *>(item);
    int           numLeft = itemSize;
    while (numLeft) {
        int numWritten = stream->WriteBlocking(source, numLeft);
        if (numWritten) {
            source  += numWritten;
            numLeft -= numWritten;
        } else {
            assert (stream->ErrorState());
            return;
        }
    }
}

BlockingOutStreamInterface &operator<<(BlockingOutStreamInterface &stream, const string &value) {
    StringTools::Serialize(value, &stream);
    return stream;
}

}    // Namespace Core.
}    // namespace K.
