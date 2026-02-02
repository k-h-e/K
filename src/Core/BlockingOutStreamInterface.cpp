/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#include <K/Core/BlockingOutStreamInterface.h>

#include <cassert>

#include <K/Core/BlockingInStreamInterface.h>
#include <K/Core/ByteSpanInterface.h>
#include <K/Core/StringTools.h>

using std::string;

namespace K {
namespace Core {

void WriteItem(BlockingOutStreamInterface &stream, const void *item, int itemSize) {
    assert(itemSize > 0);
    const uint8_t *source { static_cast<const uint8_t *>(item) };
    int           numLeft { itemSize };
    while (numLeft) {
        int numWritten { stream.WriteBlocking(source, numLeft) };
        if (numWritten) {
            source  += numWritten;
            numLeft -= numWritten;
        } else {
            assert(stream.ErrorState());
            return;
        }
    }
}

bool Transfer(BlockingInStreamInterface &inStream, BlockingOutStreamInterface &outStream, int numBytes,
              ByteSpanInterface &buffer) {
    bool success      { true };
    int  numRemaining { numBytes };
    while ((numRemaining > 0) && success) {
        int numToRead { numRemaining < buffer.ByteSpanSize() ? numRemaining : buffer.ByteSpanSize() };
        int numRead { inStream.ReadBlocking(buffer.ByteSpanStart(), numToRead) };
        if (!inStream.ErrorState() && (numRead > 0)) {
            WriteItem(outStream, buffer.ByteSpanStart(), numRead);
            if (outStream.ErrorState()) {
                success = false;
            } else {
                numRemaining -= numRead;
            }
        } else {
            success = false;
        }
    }
   
    return success;
}

BlockingOutStreamInterface &operator<<(BlockingOutStreamInterface &stream, const string &value) {
    StringTools::Serialize(value, stream);
    return stream;
}

}    // Namespace Core.
}    // namespace K.
