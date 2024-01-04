/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's C++ Crossplatform Assets                                                                        ///
//    (C) Copyright Kai Hergenröther.                                                                      //  //
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

void FunctionAction::ExecuteAction() {
    function_();
}

}    // Namespace Core.
}    // Namespace K.
