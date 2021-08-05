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
using K::Core::StringTools;
using K::GeoPositioning::RtcmParser;
using K::IO::AsyncReadInterface;
using K::IO::ConnectionIO;

namespace K {
namespace GeoPositioning {

class NtripDgnssClient::ReadHandler : public virtual AsyncReadInterface::HandlerInterface {
  public:
    ReadHandler(const shared_ptr<RtcmMessageHandlerInterface> &messageHandler);
    void OnDataRead(const void *data, int dataSize) override;
    void OnEof() override;
    void OnError() override;

  private:
    RtcmParser parser_;
};

NtripDgnssClient::NtripDgnssClient(
    const string &host, const string &mountPoint, const string &user, const string &passWord, const string &positionGga,
    const shared_ptr<RtcmMessageHandlerInterface> &messageHandler, const shared_ptr<ConnectionIO> &connectionIO)
        : connection_(host, connectionIO),
          readHandler_(make_shared<ReadHandler>(messageHandler)) {
    connection_.Register(readHandler_);

    stringstream request;
    request << "GET /" << mountPoint << " HTTP/1.1\r\n";
    request << "Accept: rtk/rtcm, dgps/rtcm\r\n";
    request << "User-Agent: NTRIP EchoBot/0.1.0\r\n";
    request << "Authorization: Basic " << StringTools::ToBase64(user + ":" + passWord) << "\r\n";
    request << "\r\n";
    request << positionGga << "\r\n";

    auto requestText = request.str();
    Log::Print(Log::Level::Debug, this, [&]{ return "sending request: " + requestText; });
    connection_.WriteItem(requestText.c_str(), static_cast<int>(requestText.length()));
}

NtripDgnssClient::~NtripDgnssClient() {
    connection_.Unregister(readHandler_);
}

NtripDgnssClient::ReadHandler::ReadHandler(const shared_ptr<RtcmMessageHandlerInterface> &messageHandler)
        : parser_(messageHandler) {
    // Nop.
}

void NtripDgnssClient::ReadHandler::OnDataRead(const void *data, int dataSize) {
    parser_.OnDataRead(data, dataSize);
}

void NtripDgnssClient::ReadHandler::OnEof() {
    Log::Print(Log::Level::Debug, this, []{ return "EOF"; });
}

void NtripDgnssClient::ReadHandler::OnError() {
    Log::Print(Log::Level::Error, this, []{ return "error"; });
}

}    // Namespace GeoPositioning.
}    // Namespace K.
