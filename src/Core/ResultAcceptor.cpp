/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#include <K/Core/ResultAcceptor.h>

using std::mutex;
using std::string;
using std::unique_lock;

namespace K {
namespace Core {

ResultAcceptor::ResultAcceptor()
        : success_(false),
          valid_(false) {
    // Nop.
}

void ResultAcceptor::OnSuccess() {
    unique_lock<mutex> critical(lock_);    // Critical section .........................................................
    if (!valid_) {
        success_ = true;
        valid_   = true;
    }
}    // ......................................................................................... critical section, end.

void ResultAcceptor::OnFailure() {
    unique_lock<mutex> critical(lock_);    // Critical section .........................................................
    success_ = false;
    valid_   = true;
}    // ......................................................................................... critical section, end.

bool ResultAcceptor::Success() {
    unique_lock<mutex> critical(lock_);    // Critical section .........................................................
    return valid_ ? success_ : false;
}    // ......................................................................................... critical section, end.

bool ResultAcceptor::Failure() {
    unique_lock<mutex> critical(lock_);    // Critical section .........................................................
    return valid_ ? !success_ : false;
}    // ......................................................................................... critical section, end.

bool ResultAcceptor::Unset() {
    unique_lock<mutex> critical(lock_);    // Critical section .........................................................
    return !valid_;
}    // ......................................................................................... critical section, end.

string ResultAcceptor::ToString() {
    unique_lock<mutex> critical(lock_);    // Critical section .........................................................
    return valid_ ? (success_ ? "success" : "failure") : "not set";
}    // ......................................................................................... critical section, end.

}    // Namespace Core.
}    // Namespace K.
