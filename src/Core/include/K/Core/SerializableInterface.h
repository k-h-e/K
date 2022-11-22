////    ////
////   ////     K Crossplatform C++ Assets
////  ////      (C) Copyright Kai Hergenröther
//// ////
////////        - Core -
//// ////
////  ////
////   ////
////    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef K_CORE_SERIALIZABLEINTERFACE_H_
#define K_CORE_SERIALIZABLEINTERFACE_H_

#include <K/Core/Interface.h>

namespace K {
namespace Core {

class BinaryReaderInterface;
class BinaryWriterInterface;

//! Interface to entities capable of serialization and deserialization.
class SerializableInterface : public virtual Interface {
  public:
    //! Serializes the object to the specified binary writer.
    /*!
     *  The operation was successful only if the writer is not in write failed state afterwards.
     */
    virtual void Serialize(BinaryWriterInterface *writer) const = 0;
    //! Deserializes the object from the specified binary reader.
    /*!
     *  The operation was successful only if the reader is not in read failed state afterwards.
     */
    virtual void Deserialize(BinaryReaderInterface *reader) = 0;
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_SERIALIZABLEINTERFACE_H_
