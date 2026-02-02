/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_CORE_SAFELYSERIALIZABLEINTERFACE_H_
#define K_CORE_SAFELYSERIALIZABLEINTERFACE_H_

#include <K/Core/SerializableInterface.h>

namespace K {
namespace Core {

//! Interface to entities capable of safe (validated) serialization and deserialization.
class SafelySerializableInterface : public virtual SerializableInterface {
  public:
    //! Deserializes and validates the object from the specified stream.
    /*!
     *  In contrast to \ref Deserialize() which is optimized for performance, this method ensures the deserialized
     *  object is in a valid state.
     * 
     *  \return <c>false</c> in case the object could not be successfully restored from the serialized state. In this
     *          case, the method will have put the object back into a valid, yet not further specified, state.
     */
    virtual bool DeserializeAndValidate(BlockingInStreamInterface &stream) = 0;
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_SAFELYSERIALIZABLEINTERFACE_H_
