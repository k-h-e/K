/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's C++ Crossplatform Assets                                                                        ///
//    (C) Copyright Kai Hergenröther. All rights reserved.                                                 //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_ECHOSOUNDING_ENVELOPE_H_
#define K_ECHOSOUNDING_ENVELOPE_H_

#include <cstdint>
#include <vector>

#include <K/Core/SerializableInterface.h>

namespace K {
namespace EchoSounding {

//! Holds a set of echo sounder envelope data.
struct Envelope : public virtual Core::SerializableInterface {
    struct Target : public virtual Core::SerializableInterface {
        int     sampleIndex;
        uint8_t amplitude;

        Target() : sampleIndex{0}, amplitude{0u} {}
        Target(int sampleIndex, uint8_t amplitude) : sampleIndex{sampleIndex}, amplitude{amplitude} {}
        // Default copy, ok.

        // SerializableInterface...
        void Serialize(Core::BlockingOutStreamInterface *stream) const override;
        void Deserialize(Core::BlockingInStreamInterface *stream) override;
    };

    std::vector<uint8_t> samples;
    float                depthPerSampleM;
    std::vector<Target>  targets;
    bool                 locked;
    float                depthM;
    int                  depthTargetIndex;
    float                depthIntegrity;    // In [0, 1].
    float                noiseFloor;        // In [0, 1].

    Envelope();
    // Default copy, ok.
    void clear();

    // SerializableInterface...
    void Serialize(Core::BlockingOutStreamInterface *stream) const override;
    void Deserialize(Core::BlockingInStreamInterface *stream) override;
};

}    // Namespace EchoSounding.
}    // Namespace K.

#endif    // K_ECHOSOUNDING_ENVELOPE_H_
