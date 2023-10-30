/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's C++ Crossplatform Assets                                                                        ///
//    (C) Copyright Kai Hergenröther. All rights reserved.                                                 //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#include <K/EchoSounding/Envelope.h>

#include <K/Core/BlockingInStreamInterface.h>
#include <K/Core/BlockingOutStreamInterface.h>

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
    (*stream) << static_cast<uint32_t>(samples.size());
    for (uint8_t sample : samples) {
        (*stream) << sample;
    }

    (*stream) << depthPerSampleM;

    (*stream) << static_cast<uint32_t>(targets.size());
    for (const Target &target : targets) {
        target.Serialize(stream);
    }

    (*stream) << locked;
    (*stream) << depthM;
    (*stream) << depthTargetIndex;
    (*stream) << depthIntegrity;
    (*stream) << noiseFloor;
}

void Envelope::Deserialize(BlockingInStreamInterface *stream) {
    uint32_t number;
    (*stream) >> number;
    samples.clear();
    for (uint32_t i = 0u; i < number; ++i) {
        uint8_t sample;
        (*stream) >> sample;
        samples.push_back(sample);
    }

    (*stream) >> depthPerSampleM;

    (*stream) >> number;
    targets.clear();
    for (uint32_t i = 0u; i < number; ++i) {
        Target target;
        target.Deserialize(stream);
        targets.push_back(target);
    }

    (*stream) >> locked;
    (*stream) >> depthM;
    (*stream) >> depthTargetIndex;
    (*stream) >> depthIntegrity;
    (*stream) >> noiseFloor;
}

// ---

void Envelope::Target::Serialize(BlockingOutStreamInterface *stream) const {
    (*stream) << sampleIndex;
    (*stream) << amplitude;
}

void Envelope::Target::Deserialize(BlockingInStreamInterface *stream) {
    (*stream) >> sampleIndex;
    (*stream) >> amplitude;
}

}    // Namespace EchoSounding.
}    // Namespace K.
