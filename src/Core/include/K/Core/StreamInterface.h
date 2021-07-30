#ifndef K_CORE_STREAMINTERFACE_H_
#define K_CORE_STREAMINTERFACE_H_

#include <memory>
#include <K/Core/ErrorStateInterface.h>

namespace K {
namespace Core {

class Result;

//! Interface to entities providing stream I/O.
class StreamInterface : public virtual K::Core::ErrorStateInterface {
  public:
    //! Tells whether the stream is (still) good.
    /*!
     *  Good means that no stream states are set whatsoever that disable parts of the stream's functionality, e.g.
     *  error state, or EOF state in case of a readable stream.
     */
    virtual bool Good() const = 0;
    //! If a result acceptor is set, the stream will use it to report a final result upon destruction (think RAII).
    /*!
     *  In case error state is not set upon stream destruction, the result acceptor will be set to success, otherwise to
     *  failure.
     *
     *  Pass <c>nullptr</c> to unregister a previously registered result acceptor.
     *
     *  A registered result acceptor will get called from the stream's destructor, and thus from the thread running that
     *  destructor.
     *
     *  Streams will allow to set the result acceptor also via their constructor.
     */
    virtual void SetFinalResultAcceptor(const std::shared_ptr<Core::Result> &resultAcceptor) = 0;
};

}    // Namespace Core.
}    // Namespace K.

#endif    // K_CORE_STREAMINTERFACE_H_
