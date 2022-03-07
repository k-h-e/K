////    ////
////   ////     K Crossplatform C++ Assets
////  ////      (C) Copyright Kai Hergenröther
//// ////
////////        - Core -
//// ////
////  ////
////   ////
////    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef K_CORE_ITEMREADINTERFACE_H_
#define K_CORE_ITEMREADINTERFACE_H_

#include <K/Core/ReadFailedInterface.h>

namespace K {
namespace Core {

//! Interface to data readers providing item reading.
class ItemReadInterface : public virtual ReadFailedInterface {
  public:
    //! Reads a binary item of specified size (in bytes).
    /*!
     *  In case the read operation fails, read failed state will be set (see <c>ReadFailedInterface::ReadFailed()</c>).
     *  If read failed state has already been set when the method gets called, the read will not execute and the method
     *  returns immediately.
     */
    virtual void ReadItem(void *item, int itemSize) = 0;
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_ITEMREADINTERFACE_H_
