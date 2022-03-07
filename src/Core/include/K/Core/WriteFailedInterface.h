////    ////
////   ////     K Crossplatform C++ Assets
////  ////      (C) Copyright Kai Hergenröther
//// ////
////////        - Core -
//// ////
////  ////
////   ////
////    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef K_CORE_WRITEFAILEDINTERFACE_H_
#define K_CORE_WRITEFAILEDINTERFACE_H_

#include <K/Core/Interface.h>

namespace K {
namespace Core {

//! Interface to data writers maintaining write failed state.
class WriteFailedInterface : public virtual Interface {
  public:
    //! Tells whether write failed state is set.
    /*!
     *  Write failed state gets set when a client executes a certain write operation provided by the data writer and
     *  that write operation fails. While set, write failed state disables certain subsequent write operations. The
     *  write operations affecting/affected by write failed state are explicitly documented for an implementing class.
     *
     *  Once set, write failed state remains set until it is explicitly cleared, e.g. via <c>ClearWriteFailed()</c>.
     */
    virtual bool WriteFailed() const = 0;
    //! Clears write failed state.
    virtual void ClearWriteFailed() = 0;
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_WRITEFAILEDINTERFACE_H_
