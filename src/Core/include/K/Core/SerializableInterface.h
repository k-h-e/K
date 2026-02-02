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
    }
}

namespace K {
namespace Core {

//! Interface to entities capable of serialization and deserialization.
/*!
 *  The plain serialization/deserialization operations provided by this interface are supposed to be optimized for
 *  speed, as they are intended to also be used for efficient application-internal communication. While this aspect is
 *  irrelevant for the serialization operation \ref Serialize(), the deserialization operation \ref Deserialize()
 *  expects to be used on valid serialized state data delivered by non-failing streams (like e.g. memory streams). As
 *  such, it will not check for or propery report any errors, and it might raise failed assertions in particularly
 *  dangerous places. See also description for \ref Deserialize().
 */
class SerializableInterface : public virtual Interface {
  public:
    //! Serializes the object to the specified stream.
    /*!
     *  The operation was successful if and only if the stream is not in error state afterwards.
     */
    virtual void Serialize(BlockingOutStreamInterface &stream) const = 0;
    //! Deserializes the object from the specified stream.
    /*!
     *  As this method is optimized for speed, see the general discussion for \ref SerializableInterface, it expects to
     *  be used on valid data delivered by non-failing streams (like e.g. memory streams).  As such, it will not check
     *  for or propery report any errors, and it might raise failed assertions in particularly dangerous places. As a
     *  result, having a sane stream after calling this method does not guarantee that the operation was successful.
     * 
     *  Only use this method exclusively on serialization data that is guaranted to not have been damaged or tempered
     *  with. Where required, use additional mechanisms to ensure that. Also, only use it on streams that guarantee to
     *  feed it this information without failing.
     * 
     *  Where this is not possible/feasible/applicable, as for example when the serialized representation is used as
     *  part of a file format that does not provide its own integrity protection, use the
     *  \ref SafelySerializableInterface::DeserializeAndValidate() operation instead.   
     */
    virtual void Deserialize(BlockingInStreamInterface &stream) = 0;
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_SERIALIZABLEINTERFACE_H_
