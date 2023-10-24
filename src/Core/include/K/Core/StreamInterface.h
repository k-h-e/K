/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's C++ Crossplatform Assets                                                                        ///
//    (C) Copyright Kai Hergenröther. All rights reserved.                                                 //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_CORE_STREAMINTERFACE_H_
#define K_CORE_STREAMINTERFACE_H_

#include <optional>

#include <K/Core/ErrorStateInterface.h>

namespace K {
namespace Core {

//! Base interface to entities providing stream I/O.
/*!
 *  <c>K</c> streams use a simple error handling scheme: Any failing stream operation, whithout exception, makes the
 *  stream enter error state (see <c>ErrorState()</c>).
 *
 *  Because error state disables any subsequent operations and is permanent, clients can issue a whole sequence of
 *  stream operations and test for error only in the end - without risking any undefined behavior or overlooking the
 *  fact that an error has occurred.
 */
class StreamInterface : public virtual ErrorStateInterface {
  public:
    enum class Error { IO,           //!< A previous stream operation failed due to an I/O error.
                       Eof,          //!< A previous stream operation failed because hitting end-of-file.
                       User,         //!< A previous stream operation failed because of a user error.
                       Unspecific    //!< A previous stream operation failed for unspecific reasons.
    };

    //! Gives details on the stream's error state (see <c>ErrorState()</c>).
    /*!
     *  \return <c>std::nullopt</c> in case the stream is not in ErrorState(). Otherwise, an error is returned that
     *          indicates the initial cause why the stream has entered error state. Note that a typical such case would
     *          be that a previous stream operation failed because the stream hit end-of-file.
     */
    virtual std::optional<Error> StreamError() const = 0;
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_STREAMINTERFACE_H_
