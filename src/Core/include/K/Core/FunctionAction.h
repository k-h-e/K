/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther.                                                                      //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_CORE_FUNCTIONACTION_H_
#define K_CORE_FUNCTIONACTION_H_

#include <functional>
#include <K/Core/ActionInterface.h>

namespace K {
namespace Core {

//! Action calling a <c>std::function</c>.
class FunctionAction : public virtual ActionInterface {
  public:
    FunctionAction(const std::function<void()> &aFunction);
    FunctionAction()                                       = delete;
    FunctionAction(const FunctionAction &other)            = delete;
    FunctionAction &operator=(const FunctionAction &other) = delete;
    FunctionAction(FunctionAction &&other)                 = delete;
    FunctionAction &operator=(FunctionAction &&other)      = delete;

    virtual void ExecuteAction();

  private:
    std::function<void()> function_;
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_FUNCTIONACTION_H_
