/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's C++ Crossplatform Assets                                                                        ///
//    (C) Copyright Kai Hergenröther. All rights reserved.                                                 //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#include <K/IO/Deprecated/TcpConnection.h>

#include <K/Core/Log.h>
#include <K/IO/NetworkTools.h>

using std::shared_ptr;
using std::string;
using K::IO::ConnectionIO;
using K::IO::NetworkTools;

namespace K {
namespace IO {
namespace Deprecated {

TcpConnection::TcpConnection(const string &hostAndPort, const shared_ptr<ConnectionIO> &connectionIO)
        : BufferedConnection(NetworkTools::ConnectTcp(hostAndPort, nullptr), bufferSizeThreshold, connectionIO) {
    // Nop.
}

TcpConnection::TcpConnection(const string &host, int port, const shared_ptr<ConnectionIO> &connectionIO)
        : BufferedConnection(NetworkTools::ConnectTcp(host, port, nullptr), bufferSizeThreshold, connectionIO) {
    // Nop.
}

TcpConnection::TcpConnection(uint32_t ip4Address, int port, const shared_ptr<ConnectionIO> &connectionIO)
        : BufferedConnection(NetworkTools::ConnectTcp(ip4Address, port, nullptr), bufferSizeThreshold, connectionIO) {
    // Nop.
}

TcpConnection::TcpConnection(int fd, const shared_ptr<ConnectionIO> &connectionIO)
        : BufferedConnection(fd, bufferSizeThreshold, connectionIO) {
    // Nop.
}

}    // Namespace Deprecated.
}    // Namesapce IO.
}    // Namespace K.
