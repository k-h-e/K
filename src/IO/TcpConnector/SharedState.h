////    ////
////   ////     K Crossplatform C++ Assets
////  ////      (C) Copyright Kai Hergenröther
//// ////
////////        - IO -
//// ////
////  ////
////   ////
////    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef K_IO_TCPCONNECTOR_SHAREDSTATE_H_
#define K_IO_TCPCONNECTOR_SHAREDSTATE_H_

#include <condition_variable>
#include <mutex>
#include <K/Core/CompletionHandlerInterface.h>
#include <K/IO/TcpConnector.h>

namespace K {
namespace IO {

//! TCP connector state shared between threads.
/*!
 *  The class is thread safe (i.e. all its public instance methods are).
 */
class TcpConnector::SharedState : public virtual Core::CompletionHandlerInterface {
  public:
    SharedState();
    SharedState(const SharedState &other)             = delete;
    SharedState &operator=(const SharedState &other)  = delete;
    SharedState(const SharedState &&other)            = delete;
    SharedState &operator=(const SharedState &&other) = delete;

    void WaitForConnectorFinished();
    void OnCompletion(int completionId) override;

  private:
    std::mutex              lock_;    // Protects everything below...
    std::condition_variable stateChanged_;
    bool                    connectorRunning_;
};

}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_TCPCONNECTOR_SHAREDSTATE_H_
