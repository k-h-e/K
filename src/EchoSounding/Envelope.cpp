/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's C++ Crossplatform Assets                                                                        ///
//    (C) Copyright Kai Hergenröther.                                                                      //  //
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
        : depthPerSampleM{0.0f} {
    // Nop.
}

void Envelope::clear() {
    samples.clear();
    depthPerSampleM = 0.0f;
    targets.clear();
    depthM.reset();
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

    bool haveDepth = depthM.has_value();
    (*stream) << haveDepth;
    if (haveDepth) {
        (*stream) << *depthM;
    }
}

void Envelope::Deserialize(BlockingInStreamInterface *stream) {
    samples.clear();
    targets.clear();
    depthM.reset();

    uint32_t number;
    (*stream) >> number;
    if (!stream->ErrorState()) {
        for (uint32_t i = 0u; i < number; ++i) {
            uint8_t sample;
            (*stream) >> sample;
            samples.push_back(sample);
        }

        (*stream) >> depthPerSampleM;

        (*stream) >> number;
        if (!stream->ErrorState()) {
            for (uint32_t i = 0u; i < number; ++i) {
                Target target;
                target.Deserialize(stream);
                targets.push_back(target);
            }

            bool haveDepth;
            (*stream) >> haveDepth;
            if (!stream->ErrorState()) {
                if (haveDepth) {
                    float aDepthM;
                    (*stream) >> aDepthM;
                    depthM = aDepthM;
                }
            }
        }
    }
}

// ---

void Envelope::Target::Serialize(BlockingOutStreamInterface *stream) const {
    (*stream) << sampleIndex;
    (*stream) << grade;
    (*stream) << marked;
}

void Envelope::Target::Deserialize(BlockingInStreamInterface *stream) {
    (*stream) >> sampleIndex;
    (*stream) >> grade;
    (*stream) >> marked;
}

}    // Namespace EchoSounding.
}    // Namespace K.
