////    ////
////   ////     K Crossplatform C++ Assets
////  ////      (C) Copyright Kai Hergenröther
//// ////
////////        - Core -
//// ////
////  ////
////   ////
////    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef K_CORE_FINALRESULTINTERFACE_H_
#define K_CORE_FINALRESULTINTERFACE_H_

#include <memory>
#include <K/Core/Interface.h>

namespace K {
namespace Core {

class ResultAcceptor;

//! Interface to entities that report a final result upon destruction.
class FinalResultInterface : public virtual Interface {
  public:
    //! If a result acceptor is set, the implementing class instance will use it to report a final result upon
    //! destruction (think RAII).
    /*!
     *  Pass <c>nullptr</c> to unregister a previously registered result acceptor.
     *
     *  It is not uncommon for implementing classes to also pass the result acceptor on to aggregated objects. As such,
     *  the result acceptor can accumulate multiple result reports, itself reporting success only if all incoming
     *  reports indicated success (and there was at least one).
     */
    virtual void SetFinalResultAcceptor(const std::shared_ptr<Core::ResultAcceptor> &resultAcceptor) = 0;
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_FINALRESULTINTERFACE_H_
