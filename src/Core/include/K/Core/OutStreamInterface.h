/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's C++ Crossplatform Assets                                                                        ///
//    (C) Copyright Kai Hergenröther.                                                                      //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_CORE_OUTSTREAMINTERFACE_H_
#define K_CORE_OUTSTREAMINTERFACE_H_

#include <memory>
#include <K/Core/StreamInterface.h>

namespace K {
namespace Core {

class ResultAcceptor;

//! Base interface to entities providing an output stream.
class OutStreamInterface : public virtual StreamInterface {
  public:
    //! Sets/unsets a close result acceptor.
    /*!
     *  For simplicity, <c>K</c> streams do not implement a separate closed state. Instead, the underlying stream is
     *  opened upon instantiation and closed upon destruction. To get informed about the success of the close operation,
     *  clients can set a close result acceptor. If one is registered with the stream, the stream will use it upon its
     *  destruction to report its final error state that includes the result of the close operation (obviously, success
     *  is reported if error state is then not set, and failure otherwise).
     *
     *  Pass <c>nullptr</c> to unregister a previously registered close result acceptor.
     *
     *  It is not uncommon for implementing classes to also pass the close result acceptor on to aggregated objects. As
     *  such, the close result acceptor can accumulate multiple close result reports, itself reporting success only if
     *  all incoming reports indicated success (and there was at least one).
     */
    virtual void SetCloseResultAcceptor(const std::shared_ptr<Core::ResultAcceptor> &resultAcceptor) = 0;
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_OUTSTREAMINTERFACE_H_
