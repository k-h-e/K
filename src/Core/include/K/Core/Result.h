#ifndef K_CORE_RESULT_H_
#define K_CORE_RESULT_H_

#include <string>
#include <K/Core/Interface.h>

namespace K {
namespace Core {

//! Holds a simple, tri-state operation result (success/failure/not set).
class Result : public virtual Interface {
  public:
    //! Initializes the result as not set.
    Result();
    //! Initializes the result as either success or failure.
    Result(bool success);
    // Default copy/move, okay.

    //! Sets the result as either success or failure.
    void Set(bool success);

    //! Tells whether the result is success.
    bool Success();
    //! Tells whether the result is success.
    bool Failure();
    //! Returns a human-readable string describing the current result state.
    std::string ToString();

  private:
    bool success_;
    bool valid_;
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_RESULT_H_
