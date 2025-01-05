/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#include <K/Events/Serialization.h>

#include <K/Core/SeekableBlockingOutStreamInterface.h>

using std::vector;
using K::Core::BlockingInStreamInterface;
using K::Core::SeekableBlockingOutStreamInterface;
using K::Core::StreamInterface;

namespace K {
namespace Events {

void Serialize(const Event &event, int typeSlot, SeekableBlockingOutStreamInterface &stream) {    
    stream << typeSlot;

    int64_t  sizePosition { stream.StreamPosition() };
    uint32_t size         { 0u };
    stream << size;

    int64_t payloadPosition { stream.StreamPosition() };
    event.Serialize(&stream);
    int64_t finalPosition { stream.StreamPosition() };
    
    size = static_cast<uint32_t>(finalPosition - payloadPosition);
    stream.Seek(sizePosition);
    stream << size;
    stream.Seek(finalPosition);
}

void DeserializeHeader(BlockingInStreamInterface &stream, int &outTypeSlot, int &outSize, bool &outEofDetected) {
    bool noErrorAtStart = !stream.ErrorState();
    stream >> outTypeSlot;
    outEofDetected = !noErrorAtStart && (stream.StreamError() == StreamInterface::Error::Eof);

    uint32_t sizeU32;
    stream >> sizeU32;
    outSize = static_cast<int>(sizeU32);
}

}    // Namespace Events.
}    // Namespace K.
