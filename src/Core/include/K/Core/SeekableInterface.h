/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's C++ Crossplatform Assets                                                                        ///
//    (C) Copyright Kai Hergenröther.                                                                      //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_CORE_SEEKABLEINTERFACE_H_
#define K_CORE_SEEKABLEINTERFACE_H_

#include <cstdint>
#include <K/Core/Interface.h>

namespace K {
namespace Core {

//! Interface to streams providing seeking.
class SeekableInterface : public virtual Interface {
  public:
    //! Sets the read/write position as specified.
    virtual void Seek(int64_t position) = 0;
    //! Recovers from EOF error state and sets the read/write position as specified.
    /*!
     *  Like <c>Seek()</c>, but first recovers the stream from error state should <c>StreamInterface::Error::Eof</c>
     *  error state be set (see <c>StreamInterface::ErrorState()</c> and see <c>StreamInterface::StreamError()</c>).
     */
    virtual void RecoverAndSeek(int64_t position) = 0;
    //! Returns the current read/write position.
    virtual int64_t StreamPosition() const = 0;
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_SEEKABLEINTERFACE_H_
