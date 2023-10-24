/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's C++ Crossplatform Assets                                                                        ///
//    (C) Copyright Kai Hergenröther. All rights reserved.                                                 //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_ECHOSOUNDING_ENVELOPEHANDLERINTERFACE_H_
#define K_ECHOSOUNDING_ENVELOPEHANDLERINTERFACE_H_

#include <K/Core/DataHandlerInterface.h>

namespace K {
    namespace EchoSounding {
        struct Envelope;
    }
}

namespace K {
namespace EchoSounding {

//! Interface to entities handling echo sounder envelope data.
class EnvelopeHandlerInterface : public virtual Core::DataHandlerInterface {
  public:
    //! Asks the handler to process the specified envelope data set.
    /*!
     *  \param id
     *  Activation ID that was registered together with the handler.
     */
    virtual void OnEnvelope(int id, const Envelope &envelope) = 0;
};

}    // Namespace EchoSounding.
}    // Namespace K.

#endif    // K_ECHOSOUNDING_ENVELOPEHANDLERINTERFACE_H_
