////    ////
////   ////     K Crossplatform C++ Assets
////  ////      (C) Copyright Kai Hergenröther
//// ////
////////        - Core -
//// ////
////  ////
////   ////
////    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef K_CORE_READFAILEDINTERFACE_H_
#define K_CORE_READFAILEDINTERFACE_H_

#include <K/Core/Interface.h>

namespace K {
namespace Core {

//! Interface to data readers maintaining read failed state.
class ReadFailedInterface : public virtual Interface {
  public:
    //! Tells whether read failed state is set.
    /*!
     *  Read failed state gets set when a client executes a certain read operation provided by the data reader and that
     *  read operation fails. While set, read failed state disables certain subsequent read operations. The read
     *  operations affecting/affected by read failed state are explicitly documented for an implementing class.
     *
     *  Once set, read failed state remains set until it is explicitly cleared, e.g. via <c>ClearReadFailed()</c>.
     */
    virtual bool ReadFailed() const = 0;
    //! Clears read failed state.
    virtual void ClearReadFailed() = 0;
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_READFAILEDINTERFACE_H_
