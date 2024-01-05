/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_ECHOSOUNDING_ENVELOPE_H_
#define K_ECHOSOUNDING_ENVELOPE_H_

#include <cstdint>
#include <optional>
#include <vector>

#include <K/Core/SerializableInterface.h>

namespace K {
namespace EchoSounding {

//! Holds a set of echo sounder envelope data.
struct Envelope : public virtual Core::SerializableInterface {
    struct Target : public virtual Core::SerializableInterface {
        int    sampleIndex;
        float  grade;
        bool   marked;

        Target() : sampleIndex{0}, grade{0.0f}, marked{false} {}
        Target(int sampleIndex, float grade, bool marked) : sampleIndex{sampleIndex}, grade{grade}, marked{marked} {}
        // Default copy, ok.

        // SerializableInterface...
        void Serialize(Core::BlockingOutStreamInterface *stream) const override;
        void Deserialize(Core::BlockingInStreamInterface *stream) override;
    };

    std::vector<uint8_t> samples;
    float                depthPerSampleM;
    std::vector<Target>  targets;
    std::optional<float> depthM;

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
