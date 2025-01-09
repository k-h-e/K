/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther                                                                       //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

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
using K::Core::IoBufferInterface;
using K::Core::IoBuffers;
using K::Core::Log;
using K::Core::RawStreamHandlerInterface;
using K::Core::StreamInterface;
using K::Core::StringTools;
using K::Core::UniqueHandle;
using K::GeoPositioning::RtcmParser;
using K::IO::ConnectionIO;

namespace K {
namespace GeoPositioning {

class NtripDgnssClient::ReadHandler : public virtual RawStreamHandlerInterface {
  public:
    ReadHandler(const shared_ptr<RtcmMessageHandlerInterface> &messageHandler);
    void OnRawStreamData(UniqueHandle<IoBufferInterface> buffer) override;
    void OnStreamError(StreamInterface::Error error) override;

  private:
    RtcmParser parser_;
};

NtripDgnssClient::NtripDgnssClient(
    const string &host, const string &mountPoint, const string &user, const string &passWord, const string &positionGga,
    const shared_ptr<RtcmMessageHandlerInterface> &messageHandler, const shared_ptr<ConnectionIO> &connectionIO,
    const shared_ptr<IoBuffers> &ioBuffers)
        : connection_{host, connectionIO, ioBuffers},
          readHandler_{make_shared<ReadHandler>(messageHandler)} {
    connection_.Register(readHandler_);

    stringstream request;
    request << "GET /" << mountPoint << " HTTP/1.1\r\n";
    request << "Accept: rtk/rtcm, dgps/rtcm\r\n";
    request << "User-Agent: NTRIP EchoBot/0.1.0\r\n";
    request << "Authorization: Basic " << StringTools::ToBase64(user + ":" + passWord) << "\r\n";
    request << "\r\n";

    auto requestText = request.str();
    WriteItem(connection_, requestText.c_str(), static_cast<int>(requestText.length()));

    SendGga(positionGga);
}

NtripDgnssClient::~NtripDgnssClient() {
    connection_.Unregister(readHandler_);
}

void NtripDgnssClient::SendGga(const std::string &gga) {
    //Log::Print(Log::Level::Debug, this, [&]{ return "sending position: \"" + gga + "\""; });
    string lineBreak = "\r\n";
    WriteItem(connection_, &gga[0], static_cast<int>(gga.size()));
    WriteItem(connection_, &lineBreak[0], static_cast<int>(lineBreak.size()));
}

NtripDgnssClient::ReadHandler::ReadHandler(const shared_ptr<RtcmMessageHandlerInterface> &messageHandler)
        : parser_(messageHandler) {
    // Nop.
}

void NtripDgnssClient::ReadHandler::OnRawStreamData(UniqueHandle<IoBufferInterface> buffer) {
    parser_.OnRawStreamData(std::move(buffer));
}

void NtripDgnssClient::ReadHandler::OnStreamError(StreamInterface::Error error) {
    parser_.OnStreamError(error);
}

}    // Namespace GeoPositioning.
}    // Namespace K.
