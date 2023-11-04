/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's C++ Crossplatform Assets                                                                        ///
//    (C) Copyright Kai Hergenröther. All rights reserved.                                                 //  //
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
using K::Core::Log;
using K::Core::StreamHandlerInterface;
using K::Core::StreamInterface;
using K::Core::StringTools;
using K::GeoPositioning::RtcmParser;
using K::IO::ConnectionIO;

namespace K {
namespace GeoPositioning {

class NtripDgnssClient::ReadHandler : public virtual StreamHandlerInterface {
  public:
    ReadHandler(const shared_ptr<RtcmMessageHandlerInterface> &messageHandler);
    void OnStreamData(int id, const void *data, int dataSize) override;
    void OnStreamEnteredErrorState(int id, StreamInterface::Error error) override;

  private:
    RtcmParser parser_;
};

NtripDgnssClient::NtripDgnssClient(
    const string &host, const string &mountPoint, const string &user, const string &passWord, const string &positionGga,
    const shared_ptr<RtcmMessageHandlerInterface> &messageHandler, const shared_ptr<ConnectionIO> &connectionIO)
        : connection_{host, connectionIO},
          readHandler_{make_shared<ReadHandler>(messageHandler)} {
    connection_.Register(readHandler_, 0);

    stringstream request;
    request << "GET /" << mountPoint << " HTTP/1.1\r\n";
    request << "Accept: rtk/rtcm, dgps/rtcm\r\n";
    request << "User-Agent: NTRIP EchoBot/0.1.0\r\n";
    request << "Authorization: Basic " << StringTools::ToBase64(user + ":" + passWord) << "\r\n";
    request << "\r\n";

    auto requestText{request.str()};
    WriteItem(&connection_, requestText.c_str(), static_cast<int>(requestText.length()));

    SendGga(positionGga);
}

NtripDgnssClient::~NtripDgnssClient() {
    connection_.Unregister(readHandler_);
}

void NtripDgnssClient::SendGga(const std::string &gga) {
    //Log::Print(Log::Level::Debug, this, [&]{ return "sending position: \"" + gga + "\""; });
    string lineBreak = "\r\n";
    WriteItem(&connection_, &gga[0], static_cast<int>(gga.size()));
    WriteItem(&connection_, &lineBreak[0], static_cast<int>(lineBreak.size()));
}

NtripDgnssClient::ReadHandler::ReadHandler(const shared_ptr<RtcmMessageHandlerInterface> &messageHandler)
        : parser_(messageHandler, 0) {
    // Nop.
}

void NtripDgnssClient::ReadHandler::OnStreamData(int id, const void *data, int dataSize) {
    (void)id;
    parser_.OnStreamData(0, data, dataSize);
}

void NtripDgnssClient::ReadHandler::OnStreamEnteredErrorState(int id, StreamInterface::Error error) {
    (void)id;
    parser_.OnStreamEnteredErrorState(0, error);
}

}    // Namespace GeoPositioning.
}    // Namespace K.
