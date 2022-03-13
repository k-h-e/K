////    ////
////   ////     K Crossplatform C++ Assets
////  ////      (C) Copyright Kai Hergenröther
//// ////
////////        - Core -
//// ////
////  ////
////   ////
////    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef K_CORE_BINARYWRITERINTERFACE_H_
#define K_CORE_BINARYWRITERINTERFACE_H_

#include <cstdint>
#include <K/Core/ErrorStateInterface.h>
#include <K/Core/FinalResultInterface.h>
#include <K/Core/ItemWriteInterface.h>

namespace K {
namespace Core {

//! Interface to binary writers.
class BinaryWriterInterface : public virtual ErrorStateInterface,
                              public virtual ItemWriteInterface,
                              public virtual FinalResultInterface {
  public:
    //! Writes an integer.
    /*!
     *  Failure behavior as with <c>ItemWriteInterface::WriteItem()</c>.
     */
    virtual void Write(int value) = 0;
    //! Writes the specified <c>uint8</c> value.
    /*!
     *  Failure behavior as with <c>ItemWriteInterface::WriteItem()</c>.
     */
    virtual void Write(uint8_t value) = 0;
    //! Writes the specified <c>uint32</c> value.
    /*!
     *  Failure behavior as with <c>ItemWriteInterface::WriteItem()</c>.
     */
    virtual void Write(uint32_t value) = 0;
    //! Writes the specified single-precision floating point number.
    /*!
     *  Failure behavior as with <c>ItemWriteInterface::WriteItem()</c>.
     */
    virtual void Write(float value) = 0;
    //! Writes the specified double-precision floating point number.
    /*!
     *  Failure behavior as with <c>ItemWriteInterface::WriteItem()</c>.
     */
    virtual void Write(double value) = 0;
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_BINARYWRITERINTERFACE_H_
