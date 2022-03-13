////    ////
////   ////     K Crossplatform C++ Assets
////  ////      (C) Copyright Kai Hergenröther
//// ////
////////        - Core -
//// ////
////  ////
////   ////
////    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef K_CORE_TEXTWRITERINTERFACE_H_
#define K_CORE_TEXTWRITERINTERFACE_H_

#include <string>
#include <K/Core/ErrorStateInterface.h>
#include <K/Core/FinalResultInterface.h>
#include <K/Core/WriteFailedInterface.h>

namespace K {
namespace Core {

//! Interface to text writers.
class TextWriterInterface : public virtual ErrorStateInterface,
                            public virtual WriteFailedInterface,
                            public virtual FinalResultInterface {
  public:
    //! Writes the specified string.
    /*!
     *  In case the write operation fails, write failed state will be set
     *  (see <c>WriteFailedInterface::WriteFailed()</c>). If write failed state has already been set when the method
     *  gets called, the write will not execute and the method returns immediately.
     */
    virtual void Write(const std::string &text) = 0;
    //! Writes the specified line of text.
    /*!
     *  In case the write operation fails, write failed state will be set
     *  (see <c>WriteFailedInterface::WriteFailed()</c>). If write failed state has already been set when the method
     *  gets called, the write will not execute and the method returns immediately.
     */
    virtual void WriteLine(const std::string &line) = 0;
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_TEXTWRITERINTERFACE_H_
