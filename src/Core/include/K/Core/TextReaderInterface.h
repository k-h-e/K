////    ////
////   ////     K Crossplatform C++ Assets
////  ////      (C) Copyright Kai Hergenröther
//// ////
////////        - Core -
//// ////
////  ////
////   ////
////    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef K_CORE_TEXTREADERINTERFACE_H_
#define K_CORE_TEXTREADERINTERFACE_H_

#include <string>
#include <K/Core/EofInterface.h>
#include <K/Core/ErrorStateInterface.h>
#include <K/Core/ReadFailedInterface.h>

namespace K {
namespace Core {

//! Interface to text readers.
class TextReaderInterface : public virtual EofInterface,
                            public virtual ErrorStateInterface,
                            public virtual ReadFailedInterface {
  public:
    //! Reads a string.
    /*!
     *  Encountering the specified delimiter or EOF will properly terminate reading. The next read will happen after the
     *  delimiter, or at EOF respectively.
     *
     *  In case the read operation fails, read failed state will be set
     *  (see <c>ReadFailedInterface::ReadFailed()</c>). If read failed state has already been set when the method
     *  gets called, the read will not execute and the method returns immediately.
     */
    virtual void Read(char delimiter, std::string *outString) = 0;
    //! Reads a string.
    /*!
     *  Encountering a character not in the specified valid set or EOF will properly terminate reading. The next read
     *  will happen at the offending position.
     *
     *  In case the read operation fails, read failed state will be set
     *  (see <c>ReadFailedInterface::ReadFailed()</c>). If read failed state has already been set when the method
     *  gets called, the read will not execute and the method returns immediately.
     *
     *  \param readOther
     *  If set, the complement of the specified set will be treated as the valid characters.
     */
    virtual void Read(const std::string &validCharacters, bool readOther, std::string *outString) = 0;
    //! Skips the specified characters.
    /*!
     *  Encountering a character not in the specified skip set or EOF will properly terminate skipping. The next read
     *  will happen at the offending position.
     *
     *  In case the read (skip) operation fails, read failed state will be set
     *  (see <c>ReadFailedInterface::ReadFailed()</c>). If read failed state has already been set when the method
     *  gets called, the read (skip) will not execute and the method returns immediately.
     *
     *  \param skipOther
     *  If set, the complement of the specified set will be treated as the characters to skip.
     */
    virtual void Skip(const std::string &charactersToSkip, bool skipOther) = 0;
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_TEXTREADERINTERFACE_H_
