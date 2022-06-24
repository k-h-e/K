#include <K/GeoPositioning/NtripDgnssClient.h>

#include <sstream>
#include <K/Core/Log.h>
#include <K/Core/StringTools.h>
#include <K/GeoPositioning/RtcmParser.h>

using std::shared_ptr;
using std::make_shared;
using std::string;
using std::to_string;
using std::stringstream;
using K::Core::Log;
using K::Core::StreamHandlerInterface;
using K::Core::StringTools;
using K::GeoPositioning::RtcmParser;
using K::IO::ConnectionIO;

namespace K {
namespace GeoPositioning {

class NtripDgnssClient::ReadHandler : public virtual StreamHandlerInterface {
  public:
    ReadHandler(const shared_ptr<RtcmMessageHandlerInterface> &messageHandler);
    void HandleStreamData(int id, const void *data, int dataSize) override;
    void HandleEof(int id) override;
    void HandleError(int id) override;

  private:
    RtcmParser parser_;
};

NtripDgnssClient::NtripDgnssClient(
    const string &host, const string &mountPoint, const string &user, const string &passWord, const string &positionGga,
    const shared_ptr<RtcmMessageHandlerInterface> &messageHandler, const shared_ptr<ConnectionIO> &connectionIO)
        : connection_(host, connectionIO),
          readHandler_(make_shared<ReadHandler>(messageHandler)) {
    connection_.Register(readHandler_, 0);

    stringstream request;
    request << "GET /" << mountPoint << " HTTP/1.1\r\n";
    request << "Accept: rtk/rtcm, dgps/rtcm\r\n";
    request << "User-Agent: NTRIP EchoBot/0.1.0\r\n";
    request << "Authorization: Basic " << StringTools::ToBase64(user + ":" + passWord) << "\r\n";
    request << "\r\n";

    auto requestText = request.str();
    connection_.WriteItem(requestText.c_str(), static_cast<int>(requestText.length()));

    SendGga(positionGga);
}

NtripDgnssClient::~NtripDgnssClient() {
    connection_.Unregister(readHandler_);
}

void NtripDgnssClient::SendGga(const std::string &gga) {
    //Log::Print(Log::Level::Debug, this, [&]{ return "sending position: \"" + gga + "\""; });
    string lineBreak = "\r\n";
    connection_.WriteItem(&gga[0], static_cast<int>(gga.size()));
    connection_.WriteItem(&lineBreak[0], static_cast<int>(lineBreak.size()));
}

NtripDgnssClient::ReadHandler::ReadHandler(const shared_ptr<RtcmMessageHandlerInterface> &messageHandler)
        : parser_(messageHandler, 0) {
    // Nop.
}

void NtripDgnssClient::ReadHandler::HandleStreamData(int id, const void *data, int dataSize) {
    (void)id;
    parser_.HandleStreamData(0, data, dataSize);
}

void NtripDgnssClient::ReadHandler::HandleEof(int id) {
    (void)id;
    parser_.HandleEof(0);
}

void NtripDgnssClient::ReadHandler::HandleError(int id) {
    (void)id;
    parser_.HandleError(0);
}

}    // Namespace GeoPositioning.
}    // Namespace K.
