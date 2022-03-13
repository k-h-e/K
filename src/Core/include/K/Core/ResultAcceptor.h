////    ////
////   ////     K Crossplatform C++ Assets
////  ////      (C) Copyright Kai Hergenröther
//// ////
////////        - Core -
//// ////
////  ////
////   ////
////    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef K_CORE_RESULTACCEPTOR_H_
#define K_CORE_RESULTACCEPTOR_H_

#include <mutex>
#include <string>
#include <K/Core/Interface.h>

namespace K {
namespace Core {

//! Accumulates result reports, maintaining a unified, tri-state operation result (success/failure/not set).
/*!
 *  The class is thread safe, i.e. all public instance methods are.
 */
class ResultAcceptor : public virtual Interface {
  public:
    //! Initializes the result as not set.
    ResultAcceptor();
    ResultAcceptor(const ResultAcceptor &other)            = delete;
    ResultAcceptor &operator=(const ResultAcceptor &other) = delete;
    ResultAcceptor(ResultAcceptor &&other)                 = delete;
    ResultAcceptor &operator=(ResultAcceptor &&other)      = delete;
    ~ResultAcceptor()                                      = default;

    //! Only if the result has not yet been set, sets the result to success.
    /*!
     *  Once the result has been set to failure, it can no longer be set to success.
     */
    void OnSuccess();
    //! Sets the result to failure.
    void OnFailure();
    //! Tells whether the accumulated result is success.
    bool Success();
    //! Tells whether the accumulated result is success.
    bool Failure();
    //! Returns a human-readable string describing the current result state.
    std::string ToString();

  private:
    std::mutex lock_;       // Protects everything below...
    bool       success_;
    bool       valid_;
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_RESULTACCEPTOR_H_
