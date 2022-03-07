////    ////
////   ////     K Crossplatform C++ Assets
////  ////      (C) Copyright Kai Hergenröther
//// ////
////////        - Core -
//// ////
////  ////
////   ////
////    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef K_CORE_EOFINTERFACE_H_
#define K_CORE_EOFINTERFACE_H_

#include <K/Core/Interface.h>

namespace K {
namespace Core {

//! Interface to data readers providing end-of-file detection.
class EofInterface : public virtual Interface {
  public:
    //! Tells whether the data reader is currently at the end-of-file (stream) position.
    /*!
     *  If the data reader also implements <c>ErrorStateInterface</c>, it will not report EOF while in error
     *  state. That means that if the reader reports EOF, the client can conclude that the reader is not in error state.
     */
    virtual bool Eof() const = 0;
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_EOFINTERFACE_H_
