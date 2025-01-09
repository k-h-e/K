/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#include <K/Core/FunctionAction.h>

using std::function;

namespace K {
namespace Core {

FunctionAction::FunctionAction(const function<void()> &aFunction)
        : function_(aFunction) {
    // Nop.
}

void FunctionAction::Execute() {
    function_();
}

}    // Namespace Core.
}    // Namespace K.
