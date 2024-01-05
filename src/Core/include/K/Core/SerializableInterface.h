/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_CORE_SERIALIZABLEINTERFACE_H_
#define K_CORE_SERIALIZABLEINTERFACE_H_

#include <K/Core/Interface.h>

namespace K {
namespace Core {

class BlockingInStreamInterface;
class BlockingOutStreamInterface;

//! Interface to entities capable of serialization and deserialization.
class SerializableInterface : public virtual Interface {
  public:
    //! Serializes the object to the specified stream.
    /*!
     *  The operation was successful only if the stream is not in error state afterwards.
     */
    virtual void Serialize(BlockingOutStreamInterface *stream) const = 0;
    //! Deserializes the object from the specified stream.
    /*!
     *  The operation was successful only if the stream is not in error state afterwards.
     */
    virtual void Deserialize(BlockingInStreamInterface *stream) = 0;
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_SERIALIZABLEINTERFACE_H_
