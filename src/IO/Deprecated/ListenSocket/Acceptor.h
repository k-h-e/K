/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's C++ Crossplatform Assets                                                                        ///
//    (C) Copyright Kai Hergenröther. All rights reserved.                                                 //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_IO_DEPRECATED_LISTENSOCKET_ACCEPTOR_H_
#define K_IO_DEPRECATED_LISTENSOCKET_ACCEPTOR_H_

#include <K/Core/ActionInterface.h>
#include <K/IO/Deprecated/ListenSocket.h>

namespace K {
namespace IO {
namespace Deprecated {

//! Acceptor for the listen socket.
class ListenSocket::Acceptor : public virtual Core::ActionInterface {
  public:
    Acceptor(int port, SharedState *shared);
    Acceptor(const Acceptor &other)             = delete;
    Acceptor &operator=(const Acceptor &other)  = delete;
    Acceptor(Acceptor &&other)                  = delete;
    Acceptor &operator=(Acceptor &&other)       = delete;
    ~Acceptor();

    void SetErrorState();
    void ExecuteAction() override;

  private:
    void Close();

    SharedState *shared_;    // Thread safe.

    int         fd_;
    bool        error_;

};

}    // Namespace Deprecated.
}    // Namespace IO.
}    // Namespace K.

#endif    // K_IO_DEPRECATED_LISTENSOCKET_ACCEPTOR_H_
