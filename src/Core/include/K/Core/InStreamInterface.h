/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's C++ Crossplatform Assets                                                                        ///
//    (C) Copyright Kai Hergenröther. All rights reserved.                                                 //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_CORE_INSTREAMINTERFACE_H_
#define K_CORE_INSTREAMINTERFACE_H_

#include <K/Core/StreamInterface.h>

namespace K {
namespace Core {

//! Base interface to entities providing an input stream.
/*!
 *  Note that <c>K</c> input streams handle and indicate hitting EOF (which will always happen in the course of
 *  performing a stream operation) via <c>ErrorState()</c> and <c>StreamError()</c>.
 */
class InStreamInterface : public virtual StreamInterface {
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_INSTREAMINTERFACE_H_
