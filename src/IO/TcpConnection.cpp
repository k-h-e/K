/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#include <K/IO/TcpConnection.h>

#include <K/Core/Log.h>
#include <K/IO/NetworkTools.h>

using std::shared_ptr;
using std::string;
using K::Core::RunLoop;
using K::IO::ConnectionIO;
using K::IO::NetworkTools;

namespace K {
namespace IO {

TcpConnection::TcpConnection(const string &hostAndPort, const shared_ptr<RunLoop> &runLoop,
                             const shared_ptr<ConnectionIO> &connectionIO)
        : Connection{NetworkTools::ConnectTcp(hostAndPort, nullptr), bufferSizeConstraint, runLoop, connectionIO} {
    // Nop.
}

TcpConnection::TcpConnection(const string &host, int port, const shared_ptr<RunLoop> &runLoop,
                             const shared_ptr<ConnectionIO> &connectionIO)
        : Connection{NetworkTools::ConnectTcp(host, port, nullptr), bufferSizeConstraint, runLoop, connectionIO} {
    // Nop.
}

TcpConnection::TcpConnection(uint32_t ip4Address, int port, const shared_ptr<RunLoop> &runLoop,
                             const shared_ptr<ConnectionIO> &connectionIO)
        : Connection{NetworkTools::ConnectTcp(ip4Address, port, nullptr), bufferSizeConstraint, runLoop, connectionIO} {
    // Nop.
}

TcpConnection::TcpConnection(int fd, const shared_ptr<RunLoop> &runLoop, const shared_ptr<ConnectionIO> &connectionIO)
        : Connection{fd, bufferSizeConstraint, runLoop, connectionIO} {
    // Nop.
}

}    // Namesapce IO.
}    // Namespace K.
