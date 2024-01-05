/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_IO_NETWORKTOOLS_H_
#define K_IO_NETWORKTOOLS_H_

#include <optional>
#include <string>

namespace K {
namespace Core {
    class Interface;
}
}

namespace K {
namespace IO {

class NetworkTools {
  public:
    //! Establishes a TCP network connection to a host given by name and port, separated by a <c>':'</c>.
    /*!
     *  \return
     *  File descriptor of the created TCP socket, and <c>nullopt</c> in case of failure.
     */
    static std::optional<int> ConnectTcp(const std::string &hostAndPort, K::Core::Interface *loggingObject);
    //! Establishes a network stream connection to the specified host.
    /*!
     *  \return
     *  File descriptor of the created TCP socket, and <c>nullopt</c> in case of failure.
     */
    static std::optional<int> ConnectTcp(const std::string &host, int port, K::Core::Interface *loggingObject);
    //! Establishes a network stream connection to the specified host.
    /*!
     *  \return
     *  File descriptor of the created TCP socket, and <c>nullopt</c> in case of failure.
     */
    static std::optional<int> ConnectTcp(uint32_t ip4Address, int port, K::Core::Interface *loggingObject);
    //! Resolves the specified host name.
    /*!
     *  \return
     *  <c>false</c> in case of failure. The output parameter will then be undefined.
     */
    static bool ResolveHostName(const std::string &hostName, uint32_t *outIp4Address,
                                K::Core::Interface *loggingObject);
    //! Returns a string representation for the specified IP4 address.
    static std::string Ip4ToString(uint32_t ip4Address);

  private:
    static bool PrepareSocket(int fd, Core::Interface *loggingObject);
};

}    // Namespace IO.
}    // namespace K.

#endif    // K_IO_NETWORKTOOLS_H_
