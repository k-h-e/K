/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's C++ Crossplatform Assets                                                                        ///
//    (C) Copyright Kai Hergenröther. All rights reserved.                                                 //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#ifndef K_GEOPOSITIONING_NTRIPDGNSSCLIENT_H_
#define K_GEOPOSITIONING_NTRIPDGNSSCLIENT_H_

#include <string>
#include <K/IO/Deprecated/TcpConnection.h>

namespace K {
    namespace Core {
        class RawStreamHandlerInterface;
    }
    namespace GeoPositioning {
        class RtcmMessageHandlerInterface;
    }
}

namespace K {
namespace GeoPositioning {

//! Receives DGNSS correction data via NTRIP.
class NtripDgnssClient : public Core::Interface {
  public:
    //! The message handler will be called from an encapsuled worker thread.
    NtripDgnssClient(
        const std::string &host, const std::string &mountPoint, const std::string &user, const std::string &passWord,
        const std::string &positionGga, const std::shared_ptr<RtcmMessageHandlerInterface> &messageHandler,
        const std::shared_ptr<IO::ConnectionIO> &connectionIO);
    NtripDgnssClient(const NtripDgnssClient &other)            = delete;
    NtripDgnssClient &operator=(const NtripDgnssClient &other) = delete;
    NtripDgnssClient(NtripDgnssClient &&other)                 = delete;
    NtripDgnssClient &operator=(NtripDgnssClient &&other)      = delete;
    ~NtripDgnssClient();

    void SendGga(const std::string &gga);

  private:
    class ReadHandler;

    K::IO::Deprecated::TcpConnection                 connection_;
    std::shared_ptr<Core::RawStreamHandlerInterface> readHandler_;
};

}    // Namespace GeoPositioning.
}    // Namespace K.

#endif    // K_GEOPOSITIONING_NTRIPDGNSSCLIENT_H_
