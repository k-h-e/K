////    ////
////   ////     K Crossplatform C++ Assets
////  ////      (C) Copyright Kai Hergenröther
//// ////
////////        - Core -
//// ////
////  ////
////   ////
////    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef K_CORE_ITEMWRITEINTERFACE_H_
#define K_CORE_ITEMWRITEINTERFACE_H_

#include <K/Core/WriteFailedInterface.h>

namespace K {
namespace Core {

//! Interface to data writers providing item writing.
class ItemWriteInterface : public virtual WriteFailedInterface {
  public:
    //! Writes a binary item of specified size (in bytes).
    /*!
     *  In case the write operation fails, write failed state will be set
     *  (see <c>WriteFailedInterface::WriteFailed()</c>). If write failed state has already been set when the method
     *  gets called, the write will not execute and the method returns immediately.
     */
    virtual void WriteItem(const void *item, int itemSize) = 0;
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_ITEMWRITEINTERFACE_H_
