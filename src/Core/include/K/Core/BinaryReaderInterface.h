////    ////
////   ////     K Crossplatform C++ Assets
////  ////      (C) Copyright Kai Hergenröther
//// ////
////////        - Core -
//// ////
////  ////
////   ////
////    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef K_CORE_BINARYREADERINTERFACE_H_
#define K_CORE_BINARYREADERINTERFACE_H_

#include <cstdint>
#include <K/Core/EofInterface.h>
#include <K/Core/ErrorStateInterface.h>
#include <K/Core/ItemReadInterface.h>

namespace K {
namespace Core {

//! Interface to binary readers.
class BinaryReaderInterface : public virtual ErrorStateInterface,
                              public virtual EofInterface,
                              public virtual ItemReadInterface {
  public:
    //! Reads an integer.
    /*!
     *  Failure behavior as with <c>ItemReadInterface::ReadItem()</c>.
     */
    virtual int ReadInt() = 0;
    //! Reads an 8-bit unsigned integer.
    /*!
     *  Failure behavior as with <c>ItemReadInterface::ReadItem()</c>.
     */
    virtual uint8_t ReadUInt8() = 0;
    //! Reads a 32-bit unsigned integer.
    /*!
     *  Failure behavior as with <c>ItemReadInterface::ReadItem()</c>.
     */
    virtual uint32_t ReadUInt32() = 0;
    //! Reads a single precision floating point number.
    /*!
     *  Failure behavior as with <c>ItemReadInterface::ReadItem()</c>.
     */
    virtual float ReadFloat() = 0;
    //! Reads a double precision floating point number.
    /*!
     *  Failure behavior as with <c>ItemReadInterface::ReadItem()</c>.
     */
    virtual double ReadDouble() = 0;
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_BINARYREADERINTERFACE_H_
