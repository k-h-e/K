/////////////////////////////////////////////////////////////////////////////////////////////////////////////  //     //
//                                                                                                            //   //
//    K                                                                                                      // //
//    Kai's Reusable C++ Assets                                                                             ///
//    (C) Copyright Kai Hergenröther.                                                                      //  //
//                                                                                                        //     //
///////////////////////////////////////////////////////////////////////////////////////////////////////  //        //

#include <K/IO/Deprecated/TcpConnector.h>

#include <K/Core/Log.h>
#include <K/Core/ThreadPool.h>
#include "Connector.h"
#include "SharedState.h"

using std::make_shared;
using std::make_unique;
using std::shared_ptr;
using std::string;
using K::Core::Log;
using K::Core::ThreadPool;

namespace K {
namespace IO {
namespace Deprecated {

TcpConnector::TcpConnector(const string &hostAndPort, HandlerInterface *handler,
                           const shared_ptr<ThreadPool> &threadPool)
        : sharedState_{make_shared<SharedState>()},
          connector_{make_unique<Connector>(hostAndPort, handler)} {
    threadPool->Run(connector_.get(), sharedState_.get(), 0);
}

TcpConnector::~TcpConnector() {
    Log::Print(Log::Level::Debug, this, [&]{ return "waiting for connection attempt to finish..."; });
    sharedState_->WaitForConnectorFinished();
    Log::Print(Log::Level::Debug, this, [&]{ return "connection attempt finished"; });
}

}    // Namespace Deprecated.
}    // Namespace IO.
}    // Namespace K.
