#ifndef K_GEOPOSITIONING_NTRIPDGNSSCLIENT_H_
#define K_GEOPOSITIONING_NTRIPDGNSSCLIENT_H_

#include <string>
#include <K/IO/TcpConnection.h>

namespace K {
namespace GeoPositioning {

//! Receives DGNSS correction data via NTRIP.
class NtripDgnssClient : public K::Core::Interface {
  public:
    NtripDgnssClient(
        const std::string &host, const std::string &mountPoint, const std::string &user, const std::string &passWord,
        const std::string &positionGga, const std::shared_ptr<K::IO::ConnectionIO> &connectionIO);
    NtripDgnssClient(const NtripDgnssClient &other)            = delete;
    NtripDgnssClient &operator=(const NtripDgnssClient &other) = delete;
    NtripDgnssClient(NtripDgnssClient &&other)                 = delete;
    NtripDgnssClient &operator=(NtripDgnssClient &&other)      = delete;
    ~NtripDgnssClient();

  private:
    class ReadHandler;

    K::IO::TcpConnection                                         connection_;
    std::shared_ptr<K::IO::AsyncReadInterface::HandlerInterface> readHandler_;
};

}    // Namespace GeoPositioning.
}    // Namespace K.

#endif    // K_GEOPOSITIONING_NTRIPDGNSSCLIENT_H_
