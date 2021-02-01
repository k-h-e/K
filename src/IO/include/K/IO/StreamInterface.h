#ifndef K_IO_STREAMINTERFACE_H_
#define K_IO_STREAMINTERFACE_H_

#include <memory>
#include <K/Core/ErrorStateInterface.h>
#include <K/IO/EofInterface.h>

namespace K {

namespace Core {
    class Result;
}

//! I/O.
namespace IO {

//! Interface to entities providing stream I/O.
class StreamInterface : public virtual K::Core::ErrorStateInterface,
                        public virtual K::IO::EofInterface {
  public:
    //! If a result acceptor is set, the stream will use it to report a final result upon destruction (think RAII).
    /*!
     *  Pass <c>nullptr</c> to unregister a previously registered result acceptor.
     *
     *  A registered result acceptor will get called from the stream's destructor, and thus from the thread running that
     *  destructor.
     *
     *  Streams will allow to set the result acceptor also via their constructor.
     */
    virtual void SetFinalResultAcceptor(const std::shared_ptr<Core::Result> &resultAcceptor) = 0;
};

}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_STREAMINTERFACE_H_
