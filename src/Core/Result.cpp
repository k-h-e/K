////    ////
////   ////     K Crossplatform C++ Assets
////  ////      (C) Copyright Kai Hergenröther
//// ////
////////        - Core -
//// ////
////  ////
////   ////
////    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <K/Core/Result.h>

using std::string;

namespace K {
namespace Core {

Result::Result()
        : success_(false),
          valid_(false) {
    // Nop.
}

Result::Result(bool success)
        : success_(success),
          valid_(true) {
    // Nop.
}

void Result::Set(bool success) {
    success_ = success;
    valid_   = true;
}

bool Result::Success() {
    return valid_ ? success_ : false;
}

bool Result::Failure() {
    return valid_ ? !success_ : false;
}

string Result::ToString() {
    return valid_ ? (success_ ? "success" : "failure") : "not set";
}

}    // Namespace Core.
}    // Namespace K.
