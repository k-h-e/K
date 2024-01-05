/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_ECHOSOUNDING_ENVELOPEHANDLERINTERFACE_H_
#define K_ECHOSOUNDING_ENVELOPEHANDLERINTERFACE_H_

#include <K/Core/StreamHandlerInterface.h>

namespace K {
    namespace EchoSounding {
        struct Envelope;
    }
}

namespace K {
namespace EchoSounding {

//! Interface to entities handling echo sounder envelope data.
class EnvelopeHandlerInterface : public virtual Core::StreamHandlerInterface {
  public:
    //! Asks the handler to process the specified envelope data set.
    virtual void OnEnvelope(const Envelope &envelope) = 0;
};

}    // Namespace EchoSounding.
}    // Namespace K.

#endif    // K_ECHOSOUNDING_ENVELOPEHANDLERINTERFACE_H_
