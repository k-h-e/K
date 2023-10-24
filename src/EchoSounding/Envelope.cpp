/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's C++ Crossplatform Assets                                                                        ///
//    (C) Copyright Kai Hergenröther. All rights reserved.                                                 //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#include <K/EchoSounding/Envelope.h>

using K::Core::BlockingInStreamInterface;
using K::Core::BlockingOutStreamInterface;

namespace K {
namespace EchoSounding {

Envelope::Envelope()
        : depthPerSampleM{0.0f},
          locked{false},
          depthM{0.0f},
          depthTargetIndex{0},
          depthIntegrity{0.0f},
          noiseFloor{0.0f} {
    // Nop.
}

void Envelope::clear() {
    samples.clear();
    depthPerSampleM  = 0.0f;
    targets.clear();
    locked           = false;
    depthM           = 0.0f;
    depthTargetIndex = 0;
    depthIntegrity   = 0.0f;
    noiseFloor       = 0.0f;
}

void Envelope::Serialize(BlockingOutStreamInterface *stream) const {
    (void) stream;
}

void Envelope::Deserialize(BlockingInStreamInterface *stream) {
    (void) stream;
}

}    // Namespace EchoSounding.
}    // Namespace K.
